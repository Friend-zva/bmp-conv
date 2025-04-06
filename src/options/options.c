#include "options.h"

const double f_one[M_ONE] = {1.0};

const double f_id[M_ID * M_ID] = {
    0.0, 0.0, 0.0,
    0.0, 1.0, 0.0,
    0.0, 0.0, 0.0
};

const double f_blur_lite[M_BL * M_BL] = {
    0.0, 0.2, 0.0,
    0.2, 0.2, 0.2,
    0.0, 0.2, 0.0
};

const double f_blur_medium[M_BM * M_BM] = {
    0, 0, 1, 0, 0,
    0, 1, 1, 1, 0,
    1, 1, 1, 1, 1,
    0, 1, 1, 1, 0,
    0, 0, 1, 0, 0,
};

double factor_def = 1.0;

double bias_def = 0.0;
