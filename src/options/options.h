#pragma once

// M_ - measure
// f_ - filter

#define M_ONE (1)
extern const double f_one[M_ONE * M_ONE];

#define M_ID (3)
extern const double f_id[M_ID * M_ID];

#define M_BL (3)
extern const double f_blur_lite[M_BL * M_BL];

#define M_BM (5)
extern const double f_blur_medium[M_BM * M_BM];

extern double factor_def;

extern double bias_def;
