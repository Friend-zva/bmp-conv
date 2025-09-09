#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "convolution.h"
#include "options.h"

#define fpr_err(...) (fprintf(stderr, __VA_ARGS__))

Options *parse_opts(char **argv) {
    Options *opt = (Options *)malloc(sizeof(Options));
    if (opt == NULL) {
        fpr_err(ERROR_MALLOC);
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
        fpr_err("Filter is one of [ one | id | bl | bm ]\n");
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
            fpr_err("Factor '%s' is not number\n", argv[index_arg]);
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
            fpr_err("Bias '%s' is not number\n", argv[index_arg]);
            free_options(opt);
            return NULL;
        }
    }
    opt->bias = bias;

    return opt;
}

int main(int argc, char **argv) {
    if (argc < 7 || argc > 10) {
        fpr_err(
            "Usage: %s [files] [options] [mode]\n"
            "\nFiles (in specified order):\n"
            "  <input  file> for 'seq' and 'par' types;"
            " <input  directory> for 'queue' type\n"
            "  <output file> for 'seq' and 'par' types;"
            " <output directory> for 'queue' type\n"
            "\nOptions (in specified order):\n"
            "  <filter=[ one | id | bl | bm ]>; 'bl' - lite blur, 'bm' - "
            "medium blur\n"
            "  <factor=[ def | <double> ]>; 'def' - default\n"
            "  <bias  =[ def | <double> ]>; 'def' - default\n"
            "\nMode (in specified order):\n"
            "  <type=[ seq | par | queue | gpu ]>\n"
            "  Additional options for 'par' type:\n"
            "    <mode=[ row | column | pixel ]>\n"
            "    <number of threads=<int>>\n"
            "  Additional options for 'queue' type:\n"
            "    <number of readers=<int>>\n"
            "    <number of workers=<int>>\n"
            "    <number of writers=<int>>\n",
            argv[0]);
        return 1;
    }

    Options *opt = parse_opts(argv);
    if (opt == NULL) {
        return 1;
    }

    int index_arg = 6;
    if (strcmp(argv[index_arg], "seq") == 0) {
        if (conv_seq_mode(argv, *opt)) {
            free_options(opt);
            return 1;
        }
    } else if (strcmp(argv[index_arg], "par") == 0) {
        enum Mode mode;
        if (argc != 9) {
            fpr_err(
                "Mode and number of threads are needed for parallel "
                "convolution\n");
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
            fpr_err("Mode is one of [ row | column | pixel ]\n");
            free_options(opt);
            return 1;
        }
        index_arg++;

        int count_ths = 0;
        sscanf(argv[index_arg], "%d", &count_ths);
        if (count_ths <= 0) {
            fpr_err("Number of threads is not natural number\n");
            free_options(opt);
            return 1;
        }

        if (conv_par_mode(argv, *opt, mode, count_ths)) {
            free_options(opt);
            return 1;
        }
    } else if (strcmp(argv[index_arg], "queue") == 0) {
        if (argc != 10) {
            fpr_err(
                "Number of readers, workers and writers are needed for queue "
                "convolution\n");
            free_options(opt);
            return 1;
        }
        index_arg++;

        int count_ths[3] = {1, 1, 1};
        sscanf(argv[index_arg++], "%d", count_ths);
        sscanf(argv[index_arg++], "%d", count_ths + 1);
        sscanf(argv[index_arg], "%d", count_ths + 2);
        if (count_ths[0] <= 0 || count_ths[1] <= 0 || count_ths[2] <= 0) {
            fpr_err("Number of threads is not natural number\n");
            free_options(opt);
            return 1;
        }

        if (conv_queue_mode(argv, *opt, count_ths)) {
            free_options(opt);
            return 1;
        }
#if OpenCL_SUPPORT == 1
    } else if (strcmp(argv[index_arg], "gpu") == 0) {
        if (conv_gpu_seq_mode(argv, *opt)) {
            free_options(opt);
            return 1;
        }
#endif
    } else {
        fpr_err("Type is one of [ seq | par | queue | gpu ]\n");
        free_options(opt);
        return 1;
    }
    free_options(opt);
    return 0;
}
