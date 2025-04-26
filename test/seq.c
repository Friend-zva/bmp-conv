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
        .factor = 1.0,
        .bias = 0.0,
        .filter = &filter,
        .mode = ROW,
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
        .factor = 1.0,
        .bias = 0.0,
        .filter = &filter,
        .mode = ROW,
    };
    test_state->bmp_target_1 = conv_seq(test_state->bmp_source, opt);

    for (int y = 0; y < opt.filter->height; y++) {
        for (int x = 0; x < opt.filter->width; x++) {
            unsigned char r, g, b, r_tar, g_tar, b_tar;
            get_pixel_rgb(test_state->bmp_source, x, y, &r, &g, &b);
            get_pixel_rgb(test_state->bmp_target_1, x, y, &r_tar, &g_tar, &b_tar);

            assert_true(r == r_tar);
            assert_true(g == g_tar);
            assert_true(b == b_tar);
        }
    }
}

void bl_conv_seq_test(void **state) {
    TestState *test_state = (TestState *) *state;

    Filter filter = {
        .height = 3,
        .width = 3,
        .matrix = (double *) f_blur_lite,
    };
    Options opt = {
        .factor = 1.0,
        .bias = 0.0,
        .filter = &filter,
        .mode = ROW,
    };
    test_state->bmp_target_1 = conv_seq(test_state->bmp_source, opt);

    for (int y = 0; y < opt.filter->height; y++) {
        for (int x = 0; x < opt.filter->width; x++) {
            unsigned char r, g, b, r_tar, g_tar, b_tar;
            get_pixel_rgb(test_state->bmp_source, x, y, &r, &g, &b);
            get_pixel_rgb(test_state->bmp_target_1, x, y, &r_tar, &g_tar, &b_tar);

            assert_true(r == r_tar);
            assert_true(g == g_tar);
            assert_true(b == b_tar);
        }
    }
}


void bm_conv_seq_test(void **state) {
    TestState *test_state = (TestState *) *state;

    Filter filter = {
        .height = 5,
        .width = 5,
        .matrix = (double *) f_blur_medium,
    };
    Options opt = {
        .factor = 1.0/13.0,
        .bias = 0.0,
        .filter = &filter,
        .mode = ROW,
    };
    test_state->bmp_target_1 = conv_seq(test_state->bmp_source, opt);

    for (int y = 0; y < opt.filter->height; y++) {
        for (int x = 0; x < opt.filter->width; x++) {
            unsigned char r, g, b, r_tar, g_tar, b_tar;
            get_pixel_rgb(test_state->bmp_source, x, y, &r, &g, &b);
            get_pixel_rgb(test_state->bmp_target_1, x, y, &r_tar, &g_tar, &b_tar);

            assert_true(r == r_tar);
            assert_true(g == g_tar);
            assert_true(b == b_tar);
        }
    }
}

void eq_bm_conv_row_column_seq_test(void **state) {
    TestState *test_state = (TestState *) *state;

    Filter filter = {
        .height = 5,
        .width = 5,
        .matrix = (double *) f_blur_medium,
    };
    Options opt_1 = {
        .factor = 1.0/13.0,
        .bias = 0.0,
        .filter = &filter,
        .mode = ROW,
    };
    test_state->bmp_target_1 = conv_seq(test_state->bmp_source, opt_1);

    Options opt_2 = {
        .factor = 1.0/13.0,
        .bias = 0.0,
        .filter = &filter,
        .mode = COLUMN,
    };
    test_state->bmp_target_2 = conv_seq(test_state->bmp_source, opt_2);

    for (int y = 0; y < opt_1.filter->height; y++) {
        for (int x = 0; x < opt_1.filter->width; x++) {
            unsigned char r_tar_1, g_tar_1, b_tar_1, r_tar_2, g_tar_2, b_tar_2;
            get_pixel_rgb(test_state->bmp_target_1, x, y, &r_tar_1, &g_tar_1, &b_tar_1);
            get_pixel_rgb(test_state->bmp_target_2, x, y, &r_tar_2, &g_tar_2, &b_tar_2);

            assert_true(r_tar_1 == r_tar_2);
            assert_true(g_tar_1 == g_tar_2);
            assert_true(b_tar_1 == b_tar_2);
        }
    }
}
