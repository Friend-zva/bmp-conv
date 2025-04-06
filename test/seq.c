#include "utils.h"
#include "options.h"
#include "seq/_conv.h"
#include "seq.h"

void non_null_conv_seq_test(void **state) {
    TestState *test_state = (TestState *) *state;

    Filter filter = {
        .height = 1,
        .width = 1,
        .matrix = (double *) f_one
    };

    Options opt = {
        .factor = factor_def,
        .bias = bias_def,
        .filter = &filter
    };

    test_state->bmp_seq = conv_seq(test_state->bmp_source, opt);

    assert_non_null(test_state->bmp_seq);
}

void id_conv_seq_test(void **state) {
    TestState *test_state = (TestState *) *state;

    Filter filter = {
        .height = 3,
        .width = 3,
        .matrix = (double *) f_id
    };

    Options opt = {
        .factor = factor_def,
        .bias = bias_def,
        .filter = &filter
    };

    test_state->bmp_seq = conv_seq(test_state->bmp_source, opt);

    for (int y = 0; y < opt.filter->height; y++)
        for (int x = 0; x < opt.filter->width; x++) {
            unsigned char r, g, b, r_seq, g_seq, b_seq;
            get_pixel_rgb(test_state->bmp_source, x, y, &r, &g, &b);
            get_pixel_rgb(test_state->bmp_seq, x, y, &r_seq, &g_seq, &b_seq);

            assert_int_equal(r, r_seq);
            assert_int_equal(g, g_seq);
            assert_int_equal(b, b_seq);
        }
}

