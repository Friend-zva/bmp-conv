#pragma once

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>

#include "cmocka.h"

#include "cbmp.h"

typedef struct {
    BMP *bmp_source;
    BMP *bmp_seq;
} TestState;

int setup(void **state);

int teardown(void **state);
