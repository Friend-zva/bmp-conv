#pragma once

#include "utils/utils.h"

BMP *conv_par(BMP *bmp_source, Options opt, enum Mode mode, int count_threads);

int conv_par_mode(char **argv, Options opt, enum Mode mode, int count_threads);
