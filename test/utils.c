#include "utils.h"

#include <stdio.h>
#include <stdlib.h>

#include "config.h"

int setup(void **state) {
    TestState *test_state = (TestState *)malloc(sizeof(TestState));
    if (test_state == NULL) {
        return -1;
    }

    test_state->bmp_source = bopen(BMP_PATH);
    if (test_state->bmp_source == NULL) {
        free(test_state);
        return -1;
    }

    test_state->bmp_target_1 = NULL;
    test_state->bmp_target_2 = NULL;

    *state = test_state;
    return 0;
}

int teardown(void **state) {
    TestState *test_state = (TestState *)*state;

    if (test_state->bmp_source != NULL) {
        bclose(test_state->bmp_source);
    }
    if (test_state->bmp_target_1 != NULL) {
        bclose(test_state->bmp_target_1);
    }
    if (test_state->bmp_target_2 != NULL) {
        bclose(test_state->bmp_target_2);
    }

    free(test_state);
    return 0;
}

void assert_almost_equal(char a, char b) {
    assert_true(abs((int)a - (int)b) <= 1);
}

int rand_int_range(int min, int max) { return arc4random_uniform(max) + min; }

double rand_double_range(double min, double max) {
    return ((double)arc4random() / UINT32_MAX) * (max - min) + min;
}

Filter generate_filter_random(int measure) {
    Filter filter;
    filter.height = measure;
    filter.width = measure;

    filter.matrix = (double *)malloc(measure * measure * sizeof(double));
    if (!filter.matrix) {
        error(ERROR_MALLOC);
        exit(1);
    }

    for (int i = 0; i < measure; i++) {
        for (int j = 0; j < measure; j++) {
            filter.matrix[i * measure + j] =
                rand_double_range(DOUBLE_MIN, DOUBLE_MAX);
        }
    }

    return filter;
}

Options generate_options_random(int measure) {
    Options opt;
    Filter filter = generate_filter_random(measure);

    opt.filter = (Filter *)malloc(sizeof(Filter));
    if (!opt.filter) {
        error(ERROR_MALLOC);
        exit(1);
    }
    *opt.filter = filter;

    opt.factor = rand_double_range(DOUBLE_MIN, DOUBLE_MAX);
    opt.bias = rand_double_range(DOUBLE_MIN, DOUBLE_MAX);

    return opt;
}

enum Mode generate_mode_random(void) {
    return (enum Mode)(arc4random() % 3);  // 0 - ROW, 1 - COLUMN, 2 - PIXEL
}

void free_generation(Options opt) {
    if (opt.filter) {
        free(opt.filter->matrix);
        free(opt.filter);
    }
}
