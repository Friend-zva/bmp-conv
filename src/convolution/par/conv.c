#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include "_conv.h"

#define PH_COUNT 8

sig_atomic_t step = 0;
sig_atomic_t inaccuracy = 0;  // less than [PH_COUNT]

typedef struct {
    int id;
    BMP *bmp_source;
    BMP *bmp_conv;
    Options opt;
} data_thread;

void conv_row_par(void *_data) {
    data_thread *data = (data_thread *)_data;
    int id = (int)data->id;
    BMP *bmp_source = (BMP *)data->bmp_source;

    const int width = get_width(bmp_source), height = get_height(bmp_source);
    int index = step * id + ((inaccuracy > 0) ? id : 0);
    int y_start = index / width;
    int x_start = index % width;

    int count = step + ((inaccuracy > id) ? 1 : 0);
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
    int id = (int)data->id;
    BMP *bmp_source = (BMP *)data->bmp_source;

    const int width = get_width(bmp_source), height = get_height(bmp_source);
    int index = step * id + ((inaccuracy > 0) ? id : 0);
    int x_start = index / height;
    int y_start = index % height;

    int count = step + ((inaccuracy > id) ? 1 : 0);
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

BMP *conv_par(BMP *bmp_source, Options opt) {
    BMP *bmp_conv = b_create(bmp_source);
    if (bmp_conv == NULL) {
        return NULL;
    }

    pthread_t threads[PH_COUNT];
    data_thread data_threads[PH_COUNT];

    const unsigned int count_pixels =
        get_width(bmp_source) * get_height(bmp_source);
    step = (int)(count_pixels / (unsigned int)PH_COUNT);
    inaccuracy = (int)(count_pixels - (unsigned int)(step * PH_COUNT));

    for (int i = 0; i < PH_COUNT; ++i) {
        data_thread data = {.id = i,
                            .bmp_source = bmp_source,
                            .bmp_conv = bmp_conv,
                            .opt = opt};
        data_threads[i] = data;

        if (opt.mode == ROW) {
            if (pthread_create(threads + i, NULL, (void *)conv_row_par,
                               (void *)(data_threads + i))) {
                fprintf(stderr, "Error creating pthread\n");
                free(bmp_conv);
                return NULL;
            }
        } else {
            if (pthread_create(threads + i, NULL, (void *)conv_column_par,
                               (void *)(data_threads + i))) {
                fprintf(stderr, "Error creating pthread\n");
                free(bmp_conv);
                return NULL;
            }
        }
    }

    for (int i = 0; i < PH_COUNT; ++i) {
        if (pthread_join(*(threads + i), NULL)) {
            fprintf(stderr, "Error joining pthreads\n");
            free(bmp_conv);
            return NULL;
        }
    }

    return bmp_conv;
}
