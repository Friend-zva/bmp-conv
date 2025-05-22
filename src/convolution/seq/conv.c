#include <stdio.h>

#include "_conv.h"

void conv_row_seq(BMP *bmp, BMP *bmp_conv, Options opt) {
    const int width = get_width(bmp), height = get_height(bmp);
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            apply_filter(bmp, bmp_conv, opt, x, y);
        }
    }
}

void conv_column_seq(BMP *bmp, BMP *bmp_conv, Options opt) {
    const int width = get_width(bmp), height = get_height(bmp);
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            apply_filter(bmp, bmp_conv, opt, x, y);
        }
    }
}

BMP *conv_seq(BMP *bmp, Options opt) {
    BMP *bmp_conv = b_create(bmp);
    if (bmp_conv == NULL) {
        return NULL;
    }

    if (opt.mode == ROW || opt.mode == PIXEL) {
        conv_row_seq(bmp, bmp_conv, opt);
    } else if (opt.mode == COLUMN) {
        conv_column_seq(bmp, bmp_conv, opt);
    }

    return bmp_conv;
}
