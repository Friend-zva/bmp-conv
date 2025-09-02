#include "conv.h"

#include <stdio.h>

BMP *conv_seq(BMP *bmp, Options opt) {
    BMP *bmp_conv = b_create(bmp);
    if (bmp_conv == NULL) {
        return NULL;
    }

    const int width = get_width(bmp), height = get_height(bmp);
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            apply_filter(bmp, bmp_conv, opt, x, y);
        }
    }

    return bmp_conv;
}

int conv_seq_mode(char **argv, Options opt) {
    double time_start = get_time();

    BMP *bmp = bopen(argv[1]);
    if (bmp == NULL) {
        fpr_err("Error: opening input file failed\n");
        return 1;
    }

    BMP *bmp_conv = conv_seq(bmp, opt);
    if (bmp_conv == NULL) {
        return 1;
    }

    bwrite(bmp_conv, argv[2]);
    double time_end = get_time();

    printf("Check %s\n", argv[2]);
    fpr_log("Log: %fs spent\n", time_end - time_start);

    bclose(bmp);
    bclose(bmp_conv);
    return 0;
}
