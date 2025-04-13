#include <pthread.h>
#include <signal.h>

#include "_conv.h"

#define PH_COUNT 8

volatile sig_atomic_t step = 0;
volatile sig_atomic_t inaccuracy = 0; // less than [PH_COUNT]
//int step;
//int inaccuracy; // less than [PH_COUNT]

typedef struct {
    int id;
    BMP *bmp_source;
    BMP *bmp_conv;
    Options opt;
} data_thread;

void *conv_split(void *_data) {
    data_thread *data = (data_thread *) _data;
    int id = (int) data->id;
    BMP *bmp_source = (BMP *) data->bmp_source;
    BMP *bmp_conv = (BMP *) data->bmp_conv;
    Options opt = (Options) data->opt;

    const int width = get_width(bmp_source), height = get_height(bmp_source);
    int y_start = step * id % width;
    int x_start = step * id / height;

    if (inaccuracy > id) {
        if (x_start != width - 1)
            x_start++;
        else {
            x_start = 0;
            y_start++;
        }
    }

    int count = 0;
    for (int y = y_start; y < height; y++)
        for (int x = x_start; x < width; x++) {
            apply_filter(bmp_source, bmp_conv, opt, x, y);

            count++;
            if (count == step)
                return NULL;
        }

    return NULL;
}

BMP *conv_par(BMP *bmp_source, Options opt) {
    BMP *bmp_conv = b_create(bmp_source);
    if (bmp_conv == NULL)
        return NULL;

    pthread_t threads[PH_COUNT];
    data_thread data_threads[PH_COUNT];

    const unsigned int count_pixels = get_width(bmp_source) * get_height(bmp_source);
    step = (int) (count_pixels / (unsigned int) PH_COUNT);
    inaccuracy = (int) (count_pixels - (unsigned int) (step * PH_COUNT));

    for (int i = 0; i < PH_COUNT; ++i) {
        data_thread data = {
            .id = i,
            .bmp_source = bmp_source,
            .bmp_conv = bmp_conv,
            .opt = opt
        };
        data_threads[i] = data;
        pthread_create(threads + i, NULL, conv_split, (void *) (data_threads + i));
    }

    for (int i = 0; i < PH_COUNT; ++i) {
        pthread_join(*(threads + i), NULL);
    }

    return bmp_conv;
}
