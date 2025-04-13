#include "utils.h"
#include "options.h"
#include "par/_conv.h"
#include "par.h"

void non_null_conv_par_test(void **state) {
    TestState *test_state = (TestState *)*state;

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

    test_state->bmp_par = conv_par(test_state->bmp_source, opt);

    assert_non_null(test_state->bmp_par);
}

void id_conv_par_test(void **state) {
    TestState *test_state = (TestState *)*state;

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

    test_state->bmp_par = conv_par(test_state->bmp_source, opt);

    for (int y = 0; y < opt.filter->height; y++)
        for (int x = 0; x < opt.filter->width; x++) {
            unsigned char r, g, b, r_par, g_par, b_par;
            get_pixel_rgb(test_state->bmp_source, x, y, &r, &g, &b);
            get_pixel_rgb(test_state->bmp_par, x, y, &r_par, &g_par, &b_par);

            assert_int_equal(r, r_par);
            assert_int_equal(g, g_par);
            assert_int_equal(b, b_par);
        }
}
