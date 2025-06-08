#include "conv.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

sig_atomic_t count_ths = 0;            // count pthreads
sig_atomic_t count_pixels_per_th = 0;  // count pixels per pthread
sig_atomic_t inaccuracy =
    0;  // additional pixels for each pthread, less than [count_ths]

typedef struct {
    BMP *bmp_source;
    BMP *bmp_conv;
    Options opt;
    int id;
} data_thread;

void conv_row_par(void *_data) {
    data_thread *data = (data_thread *)_data;
    BMP *bmp_source = (BMP *)data->bmp_source;
    int id = (int)data->id;

    const int width = get_width(bmp_source), height = get_height(bmp_source);
    int index = count_pixels_per_th * id + ((inaccuracy > 0) ? id : 0);
    int y_start = index / width;
    int x_start = index % width;

    int count = count_pixels_per_th + ((inaccuracy > id) ? 1 : 0);
    for (int y = y_start; y < height; y++) {
        for (int x = x_start; x < width; x++) {
            apply_filter(bmp_source, (BMP *)data->bmp_conv, (Options)data->opt,
                         x, y);

            count--;
            if (count == 0) {
                return;
            }
        }

        x_start = 0;
    }
}

void conv_column_par(void *_data) {
    data_thread *data = (data_thread *)_data;
    BMP *bmp_source = (BMP *)data->bmp_source;
    int id = (int)data->id;

    const int width = get_width(bmp_source), height = get_height(bmp_source);
    int index = count_pixels_per_th * id + ((inaccuracy > 0) ? id : 0);
    int x_start = index / height;
    int y_start = index % height;

    int count = count_pixels_per_th + ((inaccuracy > id) ? 1 : 0);
    for (int x = x_start; x < width; x++) {
        for (int y = y_start; y < height; y++) {
            apply_filter(bmp_source, (BMP *)data->bmp_conv, (Options)data->opt,
                         x, y);

            count--;
            if (count == 0) {
                return;
            }
        }

        y_start = 0;
    }
}

void conv_pixel_par(void *_data) {
    data_thread *data = (data_thread *)_data;
    BMP *bmp_source = (BMP *)data->bmp_source;
    int id = (int)data->id;

    const int width = get_width(bmp_source), height = get_height(bmp_source);

    int count = count_pixels_per_th + ((inaccuracy > id) ? 1 : 0);
    for (int index = id; index < width * height; index += count_ths) {
        int y = index / width;
        int x = index % width;

        apply_filter(bmp_source, (BMP *)data->bmp_conv, (Options)data->opt, x,
                     y);

        count--;
        if (count == 0) {
            return;
        }
    }
}

BMP *conv_par(BMP *bmp_source, Options opt, enum Mode mode, int count_threads) {
    BMP *bmp_conv = b_create(bmp_source);
    if (bmp_conv == NULL) {
        return NULL;
    }

    pthread_t ths[count_threads];
    data_thread data_ths[count_threads];

    const unsigned int count_pixels =
        get_width(bmp_source) * get_height(bmp_source);
    count_ths = count_threads;
    count_pixels_per_th = (int)(count_pixels / (unsigned int)count_threads);
    inaccuracy = (int)(count_pixels -
                       (unsigned int)(count_pixels_per_th * count_threads));

    for (int i = 0; i < count_threads; ++i) {
        data_thread data = {
            .bmp_source = bmp_source,
            .bmp_conv = bmp_conv,
            .opt = opt,
            .id = i,
        };
        data_ths[i] = data;

        void *(*func_conv)(void *) = NULL;
        switch (mode) {
            case ROW:
                func_conv = (void *)conv_row_par;
                break;
            case COLUMN:
                func_conv = (void *)conv_column_par;
                break;
            case PIXEL:
                func_conv = (void *)conv_pixel_par;
        }
        if (pthread_create(ths + i, NULL, func_conv, (void *)(data_ths + i))) {
            error(ERROR_PTHREAD_CREATION);
            free(bmp_conv);
            return NULL;
        }
    }

    for (int i = 0; i < count_threads; ++i) {
        if (pthread_join(*(ths + i), NULL)) {
            error(ERROR_PTHREAD_JOINING);
            free(bmp_conv);
            return NULL;
        }
    }

    return bmp_conv;
}
