#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#define DEGREE (1e9)

int min(int a, int b) { return a < b ? a : b; }

int max(int a, int b) { return a > b ? a : b; }

int parse_files(DIR *dir, char **files) {
    int count = 0;
    struct dirent *file;
    while ((file = readdir(dir)) != NULL) {
        if (file->d_type == DT_REG) {
            char *name_file = file->d_name;
            char *dot = strrchr(name_file, '.');
            if (dot && strcmp(dot, ".bmp") == 0) {
                files[count++] = name_file;
            }
        }
    }

    return count;
}

double get_time(void) {
    struct timespec time;
    clock_gettime(CLOCK_MONOTONIC, &time);
    return (double)time.tv_sec + (double)time.tv_nsec / DEGREE;
}

Filter *create_filter(int measure, const double matrix[measure * measure]) {
    Filter *filter = (Filter *)malloc(sizeof(Filter));
    if (filter == NULL) {
        fpr_err(ERROR_MALLOC);
        return NULL;
    }

    filter->matrix = (double *)matrix;
    filter->height = measure;
    filter->width = measure;

    return filter;
}

void free_options(Options *opt) {
    free(opt->filter);
    free(opt);
}

BMP *b_create(BMP *b_source) {
    BMP *b_new = (BMP *)malloc(sizeof(BMP));
    if (b_new == NULL) {
        fpr_err(ERROR_MALLOC);
        return NULL;
    }

    b_new->file_byte_number = b_source->file_byte_number;
    b_new->pixel_array_start = b_source->pixel_array_start;
    b_new->width = b_source->width;
    b_new->height = b_source->height;
    b_new->depth = b_source->depth;

    b_new->file_byte_contents = (unsigned char *)malloc(
        b_new->file_byte_number * sizeof(unsigned char));
    if (b_new->file_byte_contents == NULL) {
        fpr_err(ERROR_MALLOC);
        free(b_new);
        return NULL;
    }

    for (unsigned int i = 0; i < b_new->file_byte_number; i++) {
        b_new->file_byte_contents[i] = b_source->file_byte_contents[i];
    }

    b_new->pixels =
        (pixel *)malloc(b_new->width * b_new->height * sizeof(pixel));
    if (b_new->pixels == NULL) {
        fpr_err(ERROR_MALLOC);
        free(b_new->file_byte_contents);
        free(b_new);
        return NULL;
    }

    return b_new;
}

void apply_filter(BMP *bmp, BMP *bmp_conv, Options opt, int x, int y) {
    const int width = get_width(bmp), height = get_height(bmp);
    double r_sum = 0.0, g_sum = 0.0, b_sum = 0.0;

    int center_height_f = opt.filter->height / 2;
    int center_width_f = opt.filter->width / 2;
    unsigned int index_f = 0;
    for (int y_f = 0; y_f < opt.filter->height; y_f++) {
        for (int x_f = 0; x_f < opt.filter->width; x_f++) {
            int y_loc = (y - center_height_f + y_f + height) % height;
            int x_loc = (x - center_width_f + x_f + width) % width;

            unsigned char r = 0, g = 0, b = 0;
            get_pixel_rgb(bmp, x_loc, y_loc, &r, &g, &b);

            double value = opt.filter->matrix[index_f];
            r_sum += (double)r * value;
            g_sum += (double)g * value;
            b_sum += (double)b * value;
            index_f++;
        }
    }

    set_pixel_rgb(
        bmp_conv, x, y,
        (unsigned char)min(max((int)(opt.factor * r_sum + opt.bias), 0),
                           MAX_VALUE_RGB),
        (unsigned char)min(max((int)(opt.factor * g_sum + opt.bias), 0),
                           MAX_VALUE_RGB),
        (unsigned char)min(max((int)(opt.factor * b_sum + opt.bias), 0),
                           MAX_VALUE_RGB));
}
