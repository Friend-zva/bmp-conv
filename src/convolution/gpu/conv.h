#pragma once

#include "utils/utils.h"

BMP *conv_gpu_seq(BMP *bmp, Options opt);

int conv_gpu_seq_mode(char **argv, Options opt);
