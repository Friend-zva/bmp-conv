#include <stdio.h>
#include <stdlib.h>

#include "_utils.h"

int min(int a, int b) { return a < b ? a : b; }

int max(int a, int b) { return a > b ? a : b; }

Filter *create_filter(int measure, const double matrix[measure * measure]) {
    Filter *filter = (Filter *)malloc(sizeof(Filter));
    if (filter == NULL) {
        fprintf(stderr, "Error allocating memory\n");
        return NULL;
    }

    filter->height = measure;
    filter->width = measure;
    filter->matrix = (double *)matrix;

    return filter;
}

void free_options(Options *opt) {
    free(opt->filter);
    free(opt);
}

BMP *b_create(BMP *b_source) {
    BMP *b_new = (BMP *)malloc(sizeof(BMP));
    if (b_new == NULL) {
        fprintf(stderr, "Error allocating memory\n");
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
        fprintf(stderr, "Error allocating memory\n");
        free(b_new);
        return NULL;
    }

    for (unsigned int i = 0; i < b_new->file_byte_number; i++) {
        b_new->file_byte_contents[i] = b_source->file_byte_contents[i];
    }

    b_new->pixels =
        (pixel *)malloc(b_new->width * b_new->height * sizeof(pixel));
    if (b_new->pixels == NULL) {
        fprintf(stderr, "Error allocating memory\n");
        free(b_new->file_byte_contents);
        free(b_new);
        return NULL;
    }

    return b_new;
}

void apply_filter(BMP *bmp, BMP *bmp_conv, Options opt, int x, int y) {
    const int width = get_width(bmp), height = get_height(bmp);
    unsigned char r_sum = 0, g_sum = 0, b_sim = 0;

    unsigned int index_f = 0;
    for (int y_f = 0; y_f < opt.filter->height; y_f++) {
        for (int x_f = 0; x_f < opt.filter->width; x_f++) {
            int y_loc = (y - opt.filter->height / 2 + y_f + height) % height;
            int x_loc = (x - opt.filter->width / 2 + x_f + width) % width;

            unsigned char r = 0, g = 0, b = 0;
            get_pixel_rgb(bmp, x_loc, y_loc, &r, &g, &b);

            r_sum += (unsigned char)((double)r * opt.filter->matrix[index_f]);
            g_sum += (unsigned char)((double)g * opt.filter->matrix[index_f]);
            b_sim += (unsigned char)((double)b * opt.filter->matrix[index_f]);
            index_f++;
        }
    }

    set_pixel_rgb(bmp_conv, x, y,
                  min(max((int)(opt.factor * r_sum + opt.bias), 0), MAX_VALUE),
                  min(max((int)(opt.factor * g_sum + opt.bias), 0), MAX_VALUE),
                  min(max((int)(opt.factor * b_sim + opt.bias), 0), MAX_VALUE));
}
