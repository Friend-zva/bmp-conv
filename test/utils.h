#pragma once

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include "cbmp.h"
#include "cmocka.h"
#include "utils/utils.h"

#define COUNT_THREADS_MIN (1)
#define COUNT_THREADS_MAX (16)
#define MEASURE_MATRIX_MIN (1)
#define MEASURE_MATRIX_MAX (64)
#define DOUBLE_MIN (-128.0)
#define DOUBLE_MAX (128.0)

typedef struct {
    BMP *bmp_source;
    BMP *bmp_target_1;
    BMP *bmp_target_2;
} TestState;

int setup(void **state);

int teardown(void **state);

void assert_almost_equal(char a, char b);

int rand_int_range(int min, int max);

Options generate_options_random(int measure);

enum Mode generate_mode_random(void);

void free_generation(Options opt);
