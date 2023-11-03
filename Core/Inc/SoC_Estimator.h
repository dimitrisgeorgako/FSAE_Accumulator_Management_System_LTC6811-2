/*
 * SoC_Estimator.h
 *
 *  Created on: Jul 19, 2023
 *      Author: gamin
 */

#ifndef INC_SOC_ESTIMATOR_H_
#define INC_SOC_ESTIMATOR_H_

#include <stdint.h>
#include "main.h"
#include "cmsis_os.h"
/*
 * TinyEKF: Extended Kalman Filter for embedded processors.
 */

#define N 2     // Two state values:   SoC, V1[k]
#define M 1     // Three measurements: V[k]: 'accu_oltage'

/**
  * Initializes an EKF structure.
  * @param ekf pointer to EKF structure to initialize
  * @param n number of state variables
  * @param m number of observables
  */

typedef struct
{
	double Em,R0,R1,C1,Cq,Ts;

}accu_model;


/* We can run the SoC_estimator every-time on the min_cell(.):
 * This way we make sure to track the total SoC of accumulator & estimate it's
 * voltage to make sure BMS will notify about its status */
typedef struct
{
	accu_model accu_param;

	double cell_voltage_estimate;
	double V1_estimate;
	double SoC_estimate;

}SoC_estimator;

/* EKF parameter struct */
typedef struct
{
    double x[N];    /* state vector */

    double P[N][N];  /* prediction error covariance */
    double Q[N][N];  /* process noise covariance */
    double R[M][M];  /* measurement error covariance */

    double G[N][M];  /* Kalman gain; a.k.a. K */

    double F[N][N];  /* Jacobian of process model */
    double H[M][N];  /* Jacobian of measurement model */

    double Ht[N][M]; /* transpose of measurement Jacobian */
    double Ft[N][N]; /* transpose of process Jacobian */
    double Pp[N][N]; /* P, post-prediction, pre-update */

    double fx[N];   /* output of user defined f() state-transition function */
    double hx[M];   /* output of user defined h() measurement function */

    /* temporary storage */
    double tmp0[N][N];
    double tmp1[N][M];
    double tmp2[M][N];
    double tmp3[M][M];
    double tmp4[M][M];
    double tmp5[N][N];
    double tmp6[N];
    double tmp7[M];

} ekf_t;

const float Em_Ah_LUT[404][2] = {{ 4.183, 7.637600658527782 }, { 4.178, 7.636306274555559 }, { 4.157, 7.631581852550004 }, { 4.102, 7.622829725822226 }, { 4.092, 7.618146929777781 }, { 4.089, 7.6136284077166705 }, { 4.086, 7.608025551594448 }, { 4.08, 7.601944191800004 }, { 4.078, 7.597744623555559 }, { 4.076, 7.592576476172225 }, { 4.073, 7.586890965938893 }, { 4.07, 7.58232810166667 }, { 4.067, 7.577495165972226 }, { 4.065, 7.572325385411115 }, { 4.062, 7.5663993215833365 }, { 4.06, 7.562545732505559 }, { 4.059, 7.555753085733337 }, { 4.057, 7.550955400550004 }, { 4.055, 7.546587841155559 }, { 4.054, 7.541253610955559 }, { 4.052, 7.5356041928777815 }, { 4.051, 7.530872019922226 }, { 4.049, 7.526330659194448 }, { 4.046, 7.517130514555559 }, { 4.044, 7.506422124933337 }, { 4.043, 7.497936307333337 }, { 4.041, 7.486761165772226 }, { 4.039, 7.482080140555559 }, { 4.038, 7.47205734811667 }, { 4.036, 7.462412775894448 }, { 4.034, 7.452263454155559 }, { 4.033, 7.448703396433337 }, { 4.031, 7.433087500111115 }, { 4.03, 7.428747713927781 }, { 4.028, 7.413091143222226 }, { 4.026, 7.403827770905559 }, { 4.025, 7.394531069805559 }, { 4.023, 7.379756355400003 }, { 4.022, 7.3708436623944475 }, { 4.02, 7.358814017233337 }, { 4.018, 7.348568378922225 }, { 4.017, 7.340036119494448 }, { 4.015, 7.322663598872226 }, { 4.014, 7.307656478144448 }, { 4.012, 7.292090214038892 }, { 4.01, 7.281727721422226 }, { 4.009, 7.269291302205559 }, { 4.007, 7.255890608805559 }, { 4.005, 7.246248581955559 }, { 4.004, 7.226443717238892 }, { 4.002, 7.219816148472225 }, { 4.001, 7.200659431111115 }, { 3.999, 7.18236144971667 }, { 3.997, 7.166950530988893 }, { 3.996, 7.154689549427781 }, { 3.994, 7.136281971305559 }, { 3.993, 7.118858700150003 }, { 3.991, 7.10286100631667 }, { 3.989, 7.08002465701667 }, { 3.988, 7.062999492677782 }, { 3.986, 7.042701335272226 }, { 3.985, 7.0193677039444475 }, { 3.983, 6.992594548244448 }, { 3.981, 6.965164799738893 }, { 3.98, 6.939223077738893 }, { 3.978, 6.915245587322225 }, { 3.976, 6.892980232494448 }, { 3.975, 6.8670808220777815 }, { 3.973, 6.848624381711114 }, { 3.972, 6.826156991627781 }, { 3.97, 6.816395514955559 }, { 3.968, 6.802315386661115 }, { 3.967, 6.778141476661115 }, { 3.965, 6.7670739839333365 }, { 3.964, 6.746330198694448 }, { 3.962, 6.733674957672226 }, { 3.96, 6.719244231344448 }, { 3.959, 6.712320906777782 }, { 3.957, 6.692574507638892 }, { 3.955, 6.689598710344448 }, { 3.954, 6.681053064472225 }, { 3.952, 6.679648665033337 }, { 3.951, 6.668015002533337 }, { 3.949, 6.654484881511115 }, { 3.947, 6.6467349953277814 }, { 3.946, 6.643656391711114 }, { 3.944, 6.6274579803888924 }, { 3.943, 6.618704930733337 }, { 3.941, 6.618430198766671 }, { 3.939, 6.618269089133337 }, { 3.938, 6.606764707822226 }, { 3.936, 6.598065254888892 }, { 3.935, 6.585469456272226 }, { 3.931, 6.570195698222226 }, { 3.93, 6.57394718421667 }, { 3.928, 6.557884031000004 }, { 3.926, 6.555185295011115 }, { 3.925, 6.543019597222226 }, { 3.923, 6.537112835400004 }, { 3.922, 6.532091140805559 }, { 3.92, 6.527470246133337 }, { 3.918, 6.51461423996667 }, { 3.917, 6.514756545305559 }, { 3.914, 6.493772976222226 }, { 3.912, 6.478335033550003 }, { 3.91, 6.470447184200004 }, { 3.909, 6.460125466733337 }, { 3.907, 6.453974577811114 }, { 3.906, 6.441345674972226 }, { 3.904, 6.434665379111115 }, { 3.902, 6.41906304481667 }, { 3.901, 6.410262045394448 }, { 3.899, 6.402247828427781 }, { 3.897, 6.383965739488893 }, { 3.896, 6.371592925538892 }, { 3.894, 6.369438028983337 }, { 3.893, 6.355718389088892 }, { 3.891, 6.337744772333337 }, { 3.889, 6.3373205115722255 }, { 3.888, 6.315024619355559 }, { 3.886, 6.301987340555559 }, { 3.885, 6.280680808388894 }, { 3.883, 6.262078799683337 }, { 3.881, 6.265479735222225 }, { 3.88, 6.240083787327781 }, { 3.878, 6.224094153444447 }, { 3.877, 6.196304523222226 }, { 3.875, 6.19271607472778 }, { 3.873, 6.170920516572226 }, { 3.872, 6.146866037077781 }, { 3.87, 6.125304675711114 }, { 3.868, 6.112927369677781 }, { 3.867, 6.091188440500003 }, { 3.865, 6.072567119727781 }, { 3.864, 6.040693595494448 }, { 3.862, 6.034571829833337 }, { 3.86, 6.003823620277781 }, { 3.859, 5.981100203044448 }, { 3.857, 5.95859087961667 }, { 3.856, 5.940344743594448 }, { 3.854, 5.928254033955558 }, { 3.852, 5.909582725194448 }, { 3.851, 5.879672141472226 }, { 3.849, 5.869916582744447 }, { 3.848, 5.838784175222226 }, { 3.846, 5.799942428555559 }, { 3.844, 5.782040856811115 }, { 3.843, 5.771381498211115 }, { 3.841, 5.736372235844448 }, { 3.839, 5.714453517683337 }, { 3.838, 5.69294744610556 }, { 3.836, 5.669471994400004 }, { 3.835, 5.656479522411114 }, { 3.833, 5.6363659217444475 }, { 3.831, 5.6272933774888925 }, { 3.83, 5.583001388872225 }, { 3.828, 5.552139522888893 }, { 3.827, 5.540718444855559 }, { 3.825, 5.531574026644448 }, { 3.823, 5.499202793533337 }, { 3.822, 5.473319770100003 }, { 3.82, 5.461056935650004 }, { 3.818, 5.445195115338892 }, { 3.817, 5.420550187772226 }, { 3.815, 5.395716338377781 }, { 3.814, 5.375061513700003 }, { 3.812, 5.347679797050003 }, { 3.81, 5.324510147444449 }, { 3.809, 5.300065267655558 }, { 3.807, 5.281717647105559 }, { 3.806, 5.251020370272228 }, { 3.804, 5.231840327494448 }, { 3.802, 5.219764317411115 }, { 3.801, 5.1903256336944485 }, { 3.799, 5.159808080533336 }, { 3.798, 5.142892629588892 }, { 3.796, 5.109633356088892 }, { 3.794, 5.083541930250004 }, { 3.793, 5.082885764333337 }, { 3.791, 5.059752459333337 }, { 3.789, 5.018226056172226 }, { 3.788, 5.000040952333336 }, { 3.786, 4.962000659805559 }, { 3.785, 4.943770738333337 }, { 3.783, 4.9161057856666694 }, { 3.781, 4.910852836550005 }, { 3.78, 4.887996609383337 }, { 3.778, 4.8589207947055595 }, { 3.777, 4.826453820022225 }, { 3.775, 4.798209650211114 }, { 3.773, 4.783504513688893 }, { 3.772, 4.727217485638892 }, { 3.77, 4.712439358272226 }, { 3.769, 4.67375626535556 }, { 3.767, 4.6792813517666705 }, { 3.765, 4.641655873800003 }, { 3.764, 4.612683200305561 }, { 3.762, 4.579069844288893 }, { 3.76, 4.563226721688892 }, { 3.759, 4.53098409005556 }, { 3.757, 4.501617732383337 }, { 3.756, 4.4819385916833365 }, { 3.754, 4.457050217572226 }, { 3.752, 4.397689541322226 }, { 3.751, 4.369617573005559 }, { 3.749, 4.343058893888893 }, { 3.748, 4.322371943888893 }, { 3.746, 4.276607664027781 }, { 3.744, 4.248839374822225 }, { 3.743, 4.2379382745555585 }, { 3.741, 4.228889974522225 }, { 3.74, 4.167188425222225 }, { 3.738, 4.145085337888892 }, { 3.736, 4.103530609205558 }, { 3.735, 4.054514924922226 }, { 3.733, 4.054591967783336 }, { 3.731, 3.9611891910666697 }, { 3.73, 3.9652324819888927 }, { 3.728, 3.921200342105559 }, { 3.727, 3.9020024705277816 }, { 3.725, 3.854257083444448 }, { 3.723, 3.815300584438891 }, { 3.722, 3.7808775132222254 }, { 3.72, 3.727249682094448 }, { 3.719, 3.680268736688891 }, { 3.717, 3.6674334904333366 }, { 3.715, 3.6323869155444477 }, { 3.714, 3.607845938611116 }, { 3.712, 3.5238617245333375 }, { 3.711, 3.4926613511611144 }, { 3.709, 3.429500734911114 }, { 3.707, 3.375508210400004 }, { 3.706, 3.3401604572055597 }, { 3.704, 3.2999338473888926 }, { 3.702, 3.247939654605559 }, { 3.701, 3.169845752177781 }, { 3.699, 3.1388817928444483 }, { 3.698, 3.045989321977782 }, { 3.696, 2.989085065322226 }, { 3.694, 2.9423785585722246 }, { 3.693, 2.8550302567388917 }, { 3.691, 2.8023064071722263 }, { 3.69, 2.706220209288893 }, { 3.688, 2.62547589037778 }, { 3.686, 2.5490445059555586 }, { 3.685, 2.4573723334444475 }, { 3.683, 2.3810956476722254 }, { 3.682, 2.3349202401111144 }, { 3.68, 2.2555503597500053 }, { 3.678, 2.1711652290833374 }, { 3.677, 2.1320795446 }, { 3.675, 2.0847526613944467 }, { 3.673, 2.0306945500222255 }, { 3.672, 1.973747131238893 }, { 3.67, 1.9266213823555596 }, { 3.669, 1.9096078533111136 }, { 3.667, 1.8502546198888918 }, { 3.665, 1.7928418907388934 }, { 3.664, 1.75248846620556 }, { 3.662, 1.7006882890444466 }, { 3.661, 1.693391892761114 }, { 3.659, 1.6269904620500055 }, { 3.657, 1.631064292472228 }, { 3.656, 1.551254869044449 }, { 3.654, 1.5292170681444466 }, { 3.652, 1.494427765888891 }, { 3.651, 1.5317363757722253 }, { 3.649, 1.4389406292833362 }, { 3.648, 1.4232998105111152 }, { 3.646, 1.3552436097611134 }, { 3.644, 1.3348541270000025 }, { 3.643, 1.3546870417444472 }, { 3.641, 1.2545254852888919 }, { 3.64, 1.25944114364445 }, { 3.638, 1.2597712224722262 }, { 3.636, 1.201191542933337 }, { 3.635, 1.2124661941444481 }, { 3.633, 1.1696126141666712 }, { 3.632, 1.159952594172224 }, { 3.63, 1.1118680123666707 }, { 3.628, 1.0521160581500029 }, { 3.627, 0.9892103240000045 }, { 3.625, 0.9842196525833371 }, { 3.623, 0.978737435161114 }, { 3.622, 0.8762461472777838 }, { 3.62, 0.8568382424000056 }, { 3.619, 0.7989276455888943 }, { 3.617, 0.8314278893555587 }, { 3.615, 0.7845266818888916 }, { 3.614, 0.772775783133338 }, { 3.612, 0.7465824706722257 }, { 3.611, 0.7155147342111157 }, { 3.609, 0.763155208088893 }, { 3.607, 0.6890037123500035 }, { 3.606, 0.6825816806722242 }, { 3.604, 0.6756766002666703 }, { 3.603, 0.5993708981666703 }, { 3.601, 0.6273309114833365 }, { 3.599, 0.5838174206222284 }, { 3.598, 0.591502130100003 }, { 3.596, 0.5594248506388908 }, { 3.594, 0.6192044687833373 }, { 3.593, 0.6143600521444466 }, { 3.591, 0.6244332978111142 }, { 3.59, 0.5912536529777794 }, { 3.588, 0.5972394923611137 }, { 3.585, 0.5721180964888903 }, { 3.583, 0.5456865209500048 }, { 3.582, 0.5738139142222254 }, { 3.58, 0.48583403774444633 }, { 3.578, 0.4853369903722271 }, { 3.577, 0.5497328087777813 }, { 3.575, 0.5477038214500043 }, { 3.574, 0.5467879043777817 }, { 3.572, 0.5514327801111127 }, { 3.57, 0.5199585834555576 }, { 3.569, 0.5379132756444474 }, { 3.567, 0.44907367501666773 }, { 3.565, 0.48319000493333686 }, { 3.564, 0.4710504035777827 }, { 3.561, 0.46213825877778003 }, { 3.559, 0.4791134708000033 }, { 3.557, 0.4548226999555576 }, { 3.556, 0.41866368630000483 }, { 3.553, 0.4261218137500027 }, { 3.551, 0.4548001511000024 }, { 3.548, 0.49279981775000437 }, { 3.546, 0.4550146956666703 }, { 3.545, 0.47342942256667 }, { 3.541, 0.4771349123333364 }, { 3.54, 0.48543296706111505 }, { 3.536, 0.4179664396166709 }, { 3.533, 0.4161361613666683 }, { 3.532, 0.41198464794444867 }, { 3.53, 0.44240010120000406 }, { 3.527, 0.43249963287778126 }, { 3.524, 0.40595396782222437 }, { 3.522, 0.43351157081111413 }, { 3.519, 0.4144432585444484 }, { 3.515, 0.37365458387778183 }, { 3.514, 0.40918808147222574 }, { 3.511, 0.380992962377781 }, { 3.507, 0.3711981547888925 }, { 3.506, 0.3718365120388931 }, { 3.501, 0.38035892077777955 }, { 3.499, 0.3521667472444463 }, { 3.496, 0.3937370399722244 }, { 3.491, 0.3455543581555576 }, { 3.488, 0.32475618277778207 }, { 3.486, 0.3303436290388939 }, { 3.483, 0.2840386285611114 }, { 3.478, 0.300814693666668 }, { 3.475, 0.27146414488888926 }, { 3.472, 0.2994592428500029 }, { 3.467, 0.2760874289666697 }, { 3.464, 0.2564096937277789 }, { 3.462, 0.2794122995388939 }, { 3.456, 0.24193562104444677 }, { 3.453, 0.2302667218611134 }, { 3.449, 0.25065691513334 }, { 3.445, 0.2342780577555601 }, { 3.44, 0.20916948211111652 }, { 3.437, 0.24361886766666796 }, { 3.43, 0.23934449646666955 }, { 3.427, 0.23246628422222582 }, { 3.424, 0.25894384685000205 }, { 3.417, 0.2187037037777806 }, { 3.412, 0.21616087329444866 }, { 3.409, 0.23416388048889303 }, { 3.403, 0.1975970260777844 }, { 3.398, 0.1958645247166686 }, { 3.393, 0.22752599596667267 }, { 3.387, 0.2053351723777812 }, { 3.382, 0.20687429723333395 }, { 3.377, 0.22560787901667023 }, { 3.369, 0.19514843251111547 }, { 3.364, 0.1786651966722248 }, { 3.359, 0.21287199350000297 }, { 3.354, 0.17081920236111348 }, { 3.345, 0.172309667638892 }, { 3.34, 0.1971326403833391 }, { 3.333, 0.15494228672222565 }, { 3.324, 0.1464025140888907 }, { 3.319, 0.15976227117778308 }, { 3.311, 0.12486668905000009 }, { 3.301, 0.1202335642222252 }, { 3.293, 0.13140059111666869 }, { 3.287, 0.13447660988889254 }, { 3.274, 0.1257981142722242 }, { 3.267, 0.10273465080000488 }, { 3.259, 0.12178354085000187 }, { 3.246, 0.1322919102444473 }, { 3.237, 0.10249352375555709 }, { 3.227, 0.11962797297778316 }, { 3.211, 0.11780143888889238 }, { 3.201, 0.11838785993333723 }, { 3.19, 0.0699235399111151 }, { 3.171, 0.09685497911667174 }, { 3.159, 0.056149870999999685 }, { 3.146, 0.05857950145000057 }, { 3.121, 0.024756971350003987 }, { 3.105, 0.018387392266672187 }, { 3.085, 0.0473127983000019 }, { 3.048, 0.009560164844450547 }};

void ekf_init(ekf_t *ekf);
float find_ah_from_voltage(float voltage);

/**
  * Runs one step of EKF prediction and update. Your code should first build a model, setting
  * the contents of <tt>ekf.fx</tt>, <tt>ekf.F</tt>, <tt>ekf.hx</tt>, and <tt>ekf.H</tt> to appropriate values.
  * @param ekf pointer to structure EKF
  * @param z array of measurement (observation) values
  * @return 0 on success, 1 on failure caused by non-positive-definite matrix.
  */
uint8_t SoC_update_step(ekf_t *ekf, SoC_estimator *soc, float current, float min_volt, double * z);
void    update_accu_model(double soc, accu_model *accu_param, float min_volt);

#endif /* INC_SOC_ESTIMATOR_H_ */
