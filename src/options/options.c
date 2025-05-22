#include "options.h"

// clang-format off
const double f_one[M_ONE * M_ONE] = { 1 };

const double f_id[M_ID * M_ID] = {
    0, 0, 0,
    0, 1, 0,
    0, 0, 0
};

const double f_id_shift_l[M_ID * M_ID] = {
    0, 0, 0,
    1, 0, 0,
    0, 0, 0
};

const double f_id_shift_r[M_ID * M_ID] = {
    0, 0, 0,
    0, 0, 1,
    0, 0, 0
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

const double f_extra_blur_medium[(2 + M_BM) * (2 + M_BM)] = {
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 0, 0, 0,
    0, 0, 1, 1, 1, 0, 0,
    0, 1, 1, 1, 1, 1, 0,
    0, 0, 1, 1, 1, 0, 0,
    0, 0, 0, 1, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
};
// clang-format on

#define FACTOR_DEFAULT(matrix, measure) \
    (1.0 / sum_elem_matrix((double *)matrix, measure))

double sum_elem_matrix(double *matrix, int measure) {
    double sum = 0.0;
    for (int k = 0; k < measure * measure; k++) {
        sum += matrix[k];
    }

    return sum;
}
