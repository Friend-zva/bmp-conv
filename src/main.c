#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "convolution.h"
#include "options.h"

Options *parse_opts(char **argv) {
    int index_arg = 4;

    Options *opt = (Options *) malloc(sizeof(Options));
    if (opt == NULL) {
        fprintf(stderr, "Error allocating memory\n");
        return NULL;
    }

    char mode = -1;
    if (strcmp(argv[index_arg], "row") == 0)
        mode = 0;
    else if (strcmp(argv[index_arg], "column") == 0)
        mode = 1;
    else {
        fprintf(stderr, "Mode is one of { row, column }\n");
        free(opt);
        return NULL;
    }
    opt->mode = mode;
    index_arg++;

    Filter *filter = NULL;
    if (strcmp(argv[index_arg], "one") == 0)
        filter = create_filter(M_ONE, f_one);
    else if (strcmp(argv[index_arg], "id") == 0)
        filter = create_filter(M_ID, f_id);
    else if (strcmp(argv[index_arg], "bl") == 0)
        filter = create_filter(M_BL, f_blur_lite);
    else if (strcmp(argv[index_arg], "bm") == 0)
        filter = create_filter(M_BM, f_blur_medium);
    else {
        fprintf(stderr, "Filter is one of { one, id, bl, bm }\n");
        free(opt);
        return NULL;
    }
    opt->filter = filter;
    index_arg++;

    char *error = NULL;
    double factor = strtod(argv[index_arg], &error);
    if (argv[index_arg] == error || *error != '\0') {
        fprintf(stderr, "Factor: '%s' is not number\n", argv[index_arg]);
        free(opt);
        return NULL;
    }
    opt->factor = factor;
    index_arg++;

    error = NULL;
    double bias = strtod(argv[index_arg], &error);
    if (argv[index_arg] == error || *error != '\0') {
        fprintf(stderr, "Bias: '%s' is not number\n", argv[index_arg]);
        free(opt);
        return NULL;
    }
    opt->bias = bias;

    return opt;
}


int main(int argc, char **argv) {
    if (argc != 6) {
        fprintf(stderr,
                "Usage: %s <input file> <output file> <type=seq|par> <mode=row|column> <filter=one|id|bl|bm> <factor> <bias>\n",
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
    if (strcmp(argv[3], "seq") == 0)
        bmp_conv = conv_seq(bmp, *opt);
    else if (strcmp(argv[3], "par") == 0)
        bmp_conv = conv_par(bmp, *opt);
    else {
        fprintf(stderr, "Type is one of { seq, par }\n");
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
