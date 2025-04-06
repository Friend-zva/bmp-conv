#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "convolution.h"
#include "options.h"

Options *parse_args(char **argv) {
    Options *opt = (Options *) malloc(sizeof(Options));
    if (opt == NULL) {
        fprintf(stderr, "Error allocating memory\n");
        return NULL;
    }

    Filter *filter = NULL;
    if (strcmp(argv[3], "one") == 0) {
        filter = create_filter(M_ONE, f_one);
    } else if (strcmp(argv[3], "id") == 0) {
        filter = create_filter(M_ID, f_id);
    } else if (strcmp(argv[3], "bl") == 0) {
        filter = create_filter(M_BL, f_blur_lite);
    } else if (strcmp(argv[3], "bm") == 0) {
        filter = create_filter(M_BM, f_blur_medium);
    }
    if (filter == NULL) {
        fprintf(stderr, "Filter is one of { one, id, bl, bm }\n");
        free(opt);
        return NULL;
    }
    opt->filter = filter;

    char *error = NULL;
    double factor = strtod(argv[4], &error);
    if (argv[4] == error || *error != '\0') {
        fprintf(stderr, "Factor: '%s' is not number\n", argv[4]);
        free(opt);
        return NULL;
    }
    opt->factor = factor;

    error = NULL;
    double bias = strtod(argv[5], &error);
    if (argv[5] == error || *error != '\0') {
        fprintf(stderr, "Bias: '%s' is not number\n", argv[4]);
        free(opt);
        return NULL;
    }
    opt->bias = bias;

    return opt;
}


int main(int argc, char **argv) {
    if (argc != 6) {
        fprintf(stderr,
                "Usage: %s <input file> <output file> <filter=one|id|bl|bm> <factor> <bias>\n",
                argv[0]);
        return 1;
    }

    BMP *bmp = bopen(argv[1]);
    if (bmp == NULL) {
        fprintf(stderr, "Error opening input file\n");
        return 1;
    }

    Options *opt = parse_args(argv);
    if (opt == NULL) {
        return 1;
    }

    BMP *bmp_conv = conv_seq(bmp, *opt);
    if (bmp_conv == NULL) {
        bclose(bmp);
        free(opt);
        return 1;
    }

    bwrite(bmp_conv, argv[2]);

    bclose(bmp);
    bclose(bmp_conv);
    free(opt);
    printf("Check %s\n", argv[2]);
    return 0;
}
