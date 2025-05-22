#pragma once

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include "cbmp.h"
#include "cmocka.h"

typedef struct {
    BMP *bmp_source;
    BMP *bmp_target_1;
    BMP *bmp_target_2;
} TestState;

int setup(void **state);

int teardown(void **state);

void assert_almost_equal(char a, char b);
