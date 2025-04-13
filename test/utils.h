#pragma once

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>

#include "cmocka.h"

#include "cbmp.h"

typedef struct {
    BMP *bmp_source;
    BMP *bmp_target_1;
    BMP *bmp_target_2;
} TestState;

int setup(void **state);

int teardown(void **state);
