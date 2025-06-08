#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "convolution.h"
#include "options.h"

Options *parse_opts(char **argv) {
    Options *opt = (Options *)malloc(sizeof(Options));
    if (opt == NULL) {
        fprintf(stderr, "Error allocating memory\n");
        return NULL;
    }
    int index_arg = 3;

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
    if (argc < 7 || argc > 10) {
        fprintf(stderr,
                "Usage: %s [files] [options] [mode]\n"
                "\nFiles (in specified order):\n"
                "  <input  file> for 'seq' and 'par' types;"
                " <input  directory> for 'queue' type\n"
                "  <output file> for 'seq' and 'par' types;"
                " <output directory> for 'queue' type\n"
                "\nOptions (in specified order):\n"
                "  <filter=[one|id|bl|bm>]>\n"
                "  <factor=[def|<double>]>\n"
                "  <bias  =[def|<double>]>\n"
                "\nMode (in specified order):\n"
                "  <type=[seq|par]>\n"
                "  <mode=[row|column|pixel]> only for 'par' type\n"
                "  <thread count=<int>>      only for 'par' type\n",
                argv[0]);
        return 1;
    }

    Options *opt = parse_opts(argv);
    if (opt == NULL) {
        return 1;
    }

    int index_arg = 6;
    if (strcmp(argv[index_arg], "seq") == 0) {
        BMP *bmp = bopen(argv[1]);
        if (bmp == NULL) {
            fprintf(stderr, "Error opening input file\n");
            free_options(opt);
            return 1;
        }

        BMP *bmp_conv = conv_seq(bmp, *opt);
        if (bmp_conv == NULL) {
            free_options(opt);
            return 1;
        }
        bwrite(bmp_conv, argv[2]);
        printf("Check %s\n", argv[2]);

        bclose(bmp);
        bclose(bmp_conv);
    } else if (strcmp(argv[index_arg], "par") == 0) {
        enum Mode mode;
        if (argc != 9) {
            fprintf(
                stderr,
                "Mode and thread count are needed for parallel convolution\n");
            free_options(opt);
            return 1;
        }
        index_arg++;

        if (strcmp(argv[index_arg], "row") == 0) {
            mode = ROW;
        } else if (strcmp(argv[index_arg], "column") == 0) {
            mode = COLUMN;
        } else if (strcmp(argv[index_arg], "pixel") == 0) {
            mode = PIXEL;
        } else {
            fprintf(stderr, "Mode is one of [row, column, pixel]\n");
            free_options(opt);
            return 1;
        }
        index_arg++;

        int count_th = 0;
        sscanf(argv[index_arg], "%d", &count_th);
        if (count_th <= 0) {
            fprintf(stderr, "Thread count is natural number\n");
            free_options(opt);
            return 1;
        }

        BMP *bmp = bopen(argv[1]);
        if (bmp == NULL) {
            fprintf(stderr, "Error opening input file\n");
            free_options(opt);
            return 1;
        }

        BMP *bmp_conv = conv_par(bmp, *opt, mode, count_th);
        if (bmp_conv == NULL) {
            free_options(opt);
            return 1;
        }
        bwrite(bmp_conv, argv[2]);
        printf("Check %s\n", argv[2]);

        bclose(bmp);
        bclose(bmp_conv);
    } else if (strcmp(argv[index_arg], "queue") == 0) {
        if (argc != 10) {
            fprintf(stderr, "? are needed for queue convolution\n");
            free_options(opt);
            return 1;
        }
        index_arg++;

        int count_ths[3] = {1, 1, 1};
        sscanf(argv[index_arg++], "%d", count_ths);
        sscanf(argv[index_arg++], "%d", count_ths + 1);
        sscanf(argv[index_arg], "%d", count_ths + 2);
        if (count_ths[0] <= 0 || count_ths[1] <= 0 || count_ths[2] <= 0) {
            fprintf(stderr, "Thread count is natural number\n");
            free_options(opt);
            return 1;
        }

        if (queue_mode(argv, *opt, count_ths)) {
            free_options(opt);
            return 1;
        }
    } else {
        fprintf(stderr, "Type is one of [seq, par, queue]\n");
        free_options(opt);
        return 1;
    }

    free_options(opt);
    return 0;
}
