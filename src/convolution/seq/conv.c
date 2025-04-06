#include <stdio.h>

#include "_conv.h"

BMP *conv_seq(BMP *bmp, Options opt) {
    // BMP *bmp_conv = b_deep_copy(bmp);
    BMP *bmp_conv = b_create(bmp);
    if (bmp_conv == NULL)
        return NULL;

    const int width = get_width(bmp), height = get_height(bmp);
    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
            apply_filter(bmp, bmp_conv, opt, x, y);

    return bmp_conv;
}
