#include "utils.h"
#include "options.h"
#include "seq/_conv.h"
#include "seq.h"

void non_null_conv_seq_test(void **state) {
    TestState *test_state = (TestState *) *state;

    Filter filter = {
        .height = 1,
        .width = 1,
        .matrix = (double *) f_one,
    };
    Options opt = {
        .factor = factor_def,
        .bias = bias_def,
        .filter = &filter,
        .mode = 0,
    };
    test_state->bmp_target_1 = conv_seq(test_state->bmp_source, opt);

    assert_non_null(test_state->bmp_target_1);
}

void id_conv_seq_test(void **state) {
    TestState *test_state = (TestState *) *state;

    Filter filter = {
        .height = 3,
        .width = 3,
        .matrix = (double *) f_id,
    };
    Options opt = {
        .factor = factor_def,
        .bias = bias_def,
        .filter = &filter,
        .mode = 0,
    };
    test_state->bmp_target_1 = conv_seq(test_state->bmp_source, opt);

    for (int y = 0; y < opt.filter->height; y++)
        for (int x = 0; x < opt.filter->width; x++) {
            unsigned char r, g, b, r_seq, g_seq, b_seq;
            get_pixel_rgb(test_state->bmp_source, x, y, &r, &g, &b);
            get_pixel_rgb(test_state->bmp_target_1, x, y, &r_seq, &g_seq, &b_seq);

            assert_int_equal(r, r_seq);
            assert_int_equal(g, g_seq);
            assert_int_equal(b, b_seq);
        }
}

void eq_conv_row_column_seq_test(void **state) {
    TestState *test_state = (TestState *) *state;

    Filter filter = {
        .height = 3,
        .width = 3,
        .matrix = (double *) f_id,
    };
    Options opt_1 = {
        .factor = factor_def,
        .bias = bias_def,
        .filter = &filter,
        .mode = 0,
    };
    test_state->bmp_target_1 = conv_seq(test_state->bmp_source, opt_1);

    Options opt_2 = {
        .factor = factor_def,
        .bias = bias_def,
        .filter = &filter,
        .mode = 1,
    };
    test_state->bmp_target_2 = conv_seq(test_state->bmp_source, opt_2);

    for (int y = 0; y < opt_1.filter->height; y++)
        for (int x = 0; x < opt_1.filter->width; x++) {
            unsigned char r, g, b, r_seq, g_seq, b_seq;
            get_pixel_rgb(test_state->bmp_target_1, x, y, &r, &g, &b);
            get_pixel_rgb(test_state->bmp_target_2, x, y, &r_seq, &g_seq, &b_seq);

            assert_int_equal(r, r_seq);
            assert_int_equal(g, g_seq);
            assert_int_equal(b, b_seq);
        }
}
