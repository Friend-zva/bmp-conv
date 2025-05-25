#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "convolution.h"
#include "options.h"

Options *parse_opts(char **argv) {
    int index_arg = 3;

    Options *opt = (Options *)malloc(sizeof(Options));
    if (opt == NULL) {
        fprintf(stderr, "Error allocating memory\n");
        return NULL;
    }

    Filter *filter = NULL;
    if (strcmp(argv[index_arg], "one") == 0) {
        filter = create_filter(M_ONE, f_one);
    } else if (strcmp(argv[index_arg], "id") == 0) {
        filter = create_filter(M_ID, f_id);
    } else if (strcmp(argv[index_arg], "bl") == 0) {
        filter = create_filter(M_BL, f_blur_lite);
    } else if (strcmp(argv[index_arg], "bm") == 0) {
        filter = create_filter(M_BM, f_blur_medium);
    } else {
        fprintf(stderr, "Filter is one of [one, id, bl, bm]\n");
        free(opt);
        return NULL;
    }
    if (filter == NULL) {
        free(opt);
        return NULL;
    }
    opt->filter = filter;
    index_arg++;

    double factor;
    if (strcmp(argv[index_arg], "def") == 0) {
        factor = FACTOR_DEFAULT(opt->filter->matrix, opt->filter->height);
    } else {
        char *error;
        factor = strtod(argv[index_arg], &error);
        if (argv[index_arg] == error || *error != '\0') {
            fprintf(stderr, "Factor: '%s' is not number\n", argv[index_arg]);
            free_options(opt);
            return NULL;
        }
    }
    opt->factor = factor;
    index_arg++;

    double bias;
    if (strcmp(argv[index_arg], "def") == 0) {
        bias = 0.0;
    } else {
        char *error;
        bias = strtod(argv[index_arg], &error);
        if (argv[index_arg] == error || *error != '\0') {
            fprintf(stderr, "Bias: '%s' is not number\n", argv[index_arg]);
            free_options(opt);
            return NULL;
        }
    }
    opt->bias = bias;

    return opt;
}

int main(int argc, char **argv) {
    if (argc < 7 || argc > 9) {
        fprintf(stderr,
                "Usage: %s <input file> <output file> [options] [mode]\n"
                "\nOptions (in specified order):\n"
                "  <filter=[one|id|bl|bm>]\n"
                "  <factor=[def|<double>]>\n"
                "  <bias=[def|<double>]>\n"
                "\nMode (in specified order):\n"
                "  <type=[seq|par]>\n"
                "  <mode=[row|column|pixel]> only for 'par' type\n"
                "  <thread count=<int>> only for 'par' type\n",
                argv[0]);
        return 1;
    }

    BMP *bmp = bopen(argv[1]);
    if (bmp == NULL) {
        fprintf(stderr, "Error opening input file\n");
        return 1;
    }

    Options *opt = parse_opts(argv);
    if (opt == NULL) {
        bclose(bmp);
        return 1;
    }

    BMP *bmp_conv = NULL;
    if (strcmp(argv[6], "seq") == 0) {
        bmp_conv = conv_seq(bmp, *opt);
    } else if (strcmp(argv[6], "par") == 0) {
        enum Mode mode;
        if (argc != 9) {
            fprintf(
                stderr,
                "Mode and thread count are needed for parallel convolution\n");
            bclose(bmp);
            free_options(opt);
            return 1;
        }

        if (strcmp(argv[7], "row") == 0) {
            mode = ROW;
        } else if (strcmp(argv[7], "column") == 0) {
            mode = COLUMN;
        } else if (strcmp(argv[7], "pixel") == 0) {
            mode = PIXEL;
        } else {
            fprintf(stderr, "Mode is one of [row, column, pixel]\n");
            bclose(bmp);
            free_options(opt);
            return 1;
        }

        int count_th = 0;
        sscanf(argv[8], "%d", &count_th);
        if (count_th <= 0) {
            fprintf(stderr, "Thread count is natural number\n");
            bclose(bmp);
            free_options(opt);
            return 1;
        }

        bmp_conv = conv_par(bmp, *opt, mode, count_th);
    } else {
        fprintf(stderr, "Type is one of [seq, par]\n");
        bclose(bmp);
        free_options(opt);
        return 1;
    }
    if (bmp_conv == NULL) {
        bclose(bmp);
        free_options(opt);
        return 1;
    }
    bwrite(bmp_conv, argv[2]);

    bclose(bmp);
    bclose(bmp_conv);
    free_options(opt);
    printf("Check %s\n", argv[2]);
    return 0;
}
