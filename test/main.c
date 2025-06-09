#include <stdio.h>
#include <stdlib.h>

#include "options.h"
#include "par.h"
#include "par/conv.h"
#include "seq.h"
#include "seq/conv.h"
#include "utils.h"

#define TEST_CASE(test) cmocka_unit_test_setup_teardown(test, setup, teardown)

static void eq_seq_par_bl_conv_test(void **state) {
    TestState *test_state = (TestState *)*state;

    Filter filter = {
        .height = 3,
        .width = 3,
        .matrix = (double *)f_blur_lite,
    };
    Options opt = {
        .factor = 1.0,
        .bias = 0.0,
        .filter = &filter,
    };
    test_state->bmp_target_1 = conv_seq(test_state->bmp_source, opt);
    test_state->bmp_target_2 = conv_par(test_state->bmp_source, opt, ROW, 8);

    for (int y = 0; y < opt.filter->height; y++) {
        for (int x = 0; x < opt.filter->width; x++) {
            unsigned char r_seq, g_seq, b_seq, r_par, g_par, b_par;
            get_pixel_rgb(test_state->bmp_target_1, x, y, &r_seq, &g_seq,
                          &b_seq);
            get_pixel_rgb(test_state->bmp_target_2, x, y, &r_par, &g_par,
                          &b_par);

            assert_almost_equal(r_seq, r_par);
            assert_almost_equal(g_seq, g_par);
            assert_almost_equal(b_seq, b_par);
        }
    }
}

static void eq_seq_par_random(void **state) {
    TestState *test_state = (TestState *)*state;

    int measure = rand_int_range(MEASURE_MATRIX_MIN, MEASURE_MATRIX_MAX);
    Options opt = generate_options_random(measure);
    int count_ths = rand_int_range(COUNT_THREADS_MIN, COUNT_THREADS_MAX);
    enum Mode mode = generate_mode_random();

    test_state->bmp_target_1 = conv_seq(test_state->bmp_source, opt);
    test_state->bmp_target_2 =
        conv_par(test_state->bmp_source, opt, mode, count_ths);

    for (int y = 0; y < opt.filter->height; y++) {
        for (int x = 0; x < opt.filter->width; x++) {
            unsigned char r_seq, g_seq, b_seq, r_par, g_par, b_par;
            get_pixel_rgb(test_state->bmp_target_1, x, y, &r_seq, &g_seq,
                          &b_seq);
            get_pixel_rgb(test_state->bmp_target_2, x, y, &r_par, &g_par,
                          &b_par);

            assert_almost_equal(r_seq, r_par);
            assert_almost_equal(g_seq, g_par);
            assert_almost_equal(b_seq, b_par);
        }
    }

    free_generation(opt);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        // Sequential convolution tests
        TEST_CASE(non_null_conv_seq_test),
        TEST_CASE(id_conv_seq_test),
        TEST_CASE(eq_extra_bm_conv_seq_test),
        TEST_CASE(eq_compos_bl_bm_conv_seq_test),
        TEST_CASE(eq_shift_l_r_id_conv_seq_test),
        // Parallel convolution tests
        TEST_CASE(non_null_conv_par_test),
        TEST_CASE(id_conv_par_test),
        TEST_CASE(eq_row_column_bl_conv_par_test),
        TEST_CASE(eq_row_pixel_bm_conv_par_test),
        TEST_CASE(eq_seq_par_bl_conv_test),
        // Random tests
        TEST_CASE(eq_seq_par_random),
        TEST_CASE(eq_seq_par_random),
        TEST_CASE(eq_seq_par_random),
        TEST_CASE(eq_seq_par_random),
        TEST_CASE(eq_seq_par_random),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
