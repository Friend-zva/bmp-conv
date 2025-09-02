#pragma once

#include <dirent.h>

#include "cbmp.h"

#define MAX_VALUE_RGB (255)  // 2^8 - 1

#define ERROR_MALLOC "Error: memory allocation failed\n"
#define ERROR_PTHREAD_CREATION "Error: pthread creation failed\n"
#define ERROR_PTHREAD_JOINING "Error: pthread joining failed\n"

#define fpr_err(...) (fprintf(stderr, __VA_ARGS__))
#define fpr_log(...) (fprintf(stderr, __VA_ARGS__))

typedef struct {
    double *matrix;
    int height, width;
} Filter;

enum Mode {
    ROW,
    COLUMN,
    PIXEL,
};

typedef struct {
    Filter *filter;
    double factor, bias;
} Options;

int parse_files(DIR *dir, char **files);

double get_time(void);

Filter *create_filter(int measure, const double matrix[measure * measure]);

void free_options(Options *opt);

// b_deep_copy() without copying pixels, because values of pixels always change.
BMP *b_create(BMP *b_source);

void apply_filter(BMP *bmp, BMP *bmp_conv, Options opt, int x, int y);
