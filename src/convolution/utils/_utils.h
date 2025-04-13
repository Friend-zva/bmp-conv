#pragma once

#include "cbmp.h"

typedef struct {
    int height, width;
    double *matrix;
} Filter;

typedef struct {
    double factor, bias;
    Filter *filter;
    char mode;
} Options;

#define MAX_VALUE (255) // 2^8 - 1

int min(int a, int b);

int max(int a, int b);

Filter *create_filter(int measure, const double matrix[measure * measure]);

void free_options(Options *opt);

// b_deep_copy() without copying pixels, because values of pixels always change.
BMP *b_create(BMP *b_source);

void apply_filter(BMP *bmp, BMP *bmp_conv, Options opt, int x, int y);
