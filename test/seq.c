#include "seq.h"

#include "options.h"
#include "seq/_conv.h"
#include "utils.h"

void non_null_conv_seq_test(void **state) {
    TestState *test_state = (TestState *)*state;

    Filter filter = {
        .height = M_ONE,
        .width = M_ONE,
        .matrix = (double *)f_one,
    };
    Options opt = {
        .factor = FACTOR_DEFAULT(f_one, M_ONE),
        .bias = 0.0,
        .filter = &filter,
    };
    test_state->bmp_target_1 = conv_seq(test_state->bmp_source, opt);

    assert_non_null(test_state->bmp_target_1);
}

void id_conv_seq_test(void **state) {
    TestState *test_state = (TestState *)*state;

    Filter filter = {
        .height = M_ID,
        .width = M_ID,
        .matrix = (double *)f_id,
    };
    Options opt = {
        .factor = FACTOR_DEFAULT(f_id, M_ID),
        .bias = 0.0,
        .filter = &filter,
    };
    test_state->bmp_target_1 = conv_seq(test_state->bmp_source, opt);

    for (int y = 0; y < opt.filter->height; y++) {
        for (int x = 0; x < opt.filter->width; x++) {
            unsigned char r, g, b, r_tar, g_tar, b_tar;
            get_pixel_rgb(test_state->bmp_source, x, y, &r, &g, &b);
            get_pixel_rgb(test_state->bmp_target_1, x, y, &r_tar, &g_tar,
                          &b_tar);

            assert_almost_equal(r, r_tar);
            assert_almost_equal(g, g_tar);
            assert_almost_equal(b, b_tar);
        }
    }
}

void eq_extra_bm_conv_seq_test(void **state) {
    TestState *test_state = (TestState *)*state;

    Filter filter_1 = {
        .height = M_BM,
        .width = M_BM,
        .matrix = (double *)f_blur_medium,
    };
    Options opt_1 = {
        .factor = FACTOR_DEFAULT(f_blur_medium, M_BM),
        .bias = 0.0,
        .filter = &filter_1,
    };
    test_state->bmp_target_1 = conv_seq(test_state->bmp_source, opt_1);

    Filter filter_2 = {
        .height = 2 + M_BM,
        .width = 2 + M_BM,
        .matrix = (double *)f_extra_blur_medium,
    };
    Options opt_2 = {
        .factor = FACTOR_DEFAULT(f_extra_blur_medium, 2 + M_BM),
        .bias = 0.0,
        .filter = &filter_2,
    };
    test_state->bmp_target_2 = conv_seq(test_state->bmp_source, opt_2);

    for (int y = 0; y < opt_1.filter->height; y++) {
        for (int x = 0; x < opt_1.filter->width; x++) {
            unsigned char r_tar_1, g_tar_1, b_tar_1, r_tar_2, g_tar_2, b_tar_2;
            get_pixel_rgb(test_state->bmp_target_1, x, y, &r_tar_1, &g_tar_1,
                          &b_tar_1);
            get_pixel_rgb(test_state->bmp_target_2, x, y, &r_tar_2, &g_tar_2,
                          &b_tar_2);

            assert_almost_equal(r_tar_1, r_tar_2);
            assert_almost_equal(g_tar_1, g_tar_2);
            assert_almost_equal(b_tar_1, b_tar_2);
        }
    }
}

void eq_compos_bl_bm_conv_seq_test(void **state) {
    TestState *test_state = (TestState *)*state;

    Filter filter_1 = {
        .height = M_BL,
        .width = M_BL,
        .matrix = (double *)f_blur_lite,
    };
    Options opt_1 = {
        .factor = FACTOR_DEFAULT(f_blur_lite, M_BL),
        .bias = 0.0,
        .filter = &filter_1,
    };

    Filter filter_2 = {
        .height = M_BM,
        .width = M_BM,
        .matrix = (double *)f_blur_medium,
    };
    Options opt_2 = {
        .factor = FACTOR_DEFAULT(f_blur_medium, M_BM),
        .bias = 0.0,
        .filter = &filter_2,
    };

    test_state->bmp_target_1 = conv_seq(test_state->bmp_source, opt_1);
    test_state->bmp_target_1 = conv_seq(test_state->bmp_target_1, opt_2);

    test_state->bmp_target_2 = conv_seq(test_state->bmp_source, opt_2);
    test_state->bmp_target_2 = conv_seq(test_state->bmp_target_2, opt_1);

    for (int y = 0; y < opt_1.filter->height; y++) {
        for (int x = 0; x < opt_1.filter->width; x++) {
            unsigned char r_tar_1, g_tar_1, b_tar_1, r_tar_2, g_tar_2, b_tar_2;
            get_pixel_rgb(test_state->bmp_target_1, x, y, &r_tar_1, &g_tar_1,
                          &b_tar_1);
            get_pixel_rgb(test_state->bmp_target_2, x, y, &r_tar_2, &g_tar_2,
                          &b_tar_2);

            assert_almost_equal(r_tar_1, r_tar_2);
            assert_almost_equal(g_tar_1, g_tar_2);
            assert_almost_equal(b_tar_1, b_tar_2);
        }
    }
}

void eq_shift_l_r_id_conv_seq_test(void **state) {
    TestState *test_state = (TestState *)*state;

    Filter filter_1 = {
        .height = M_ID,
        .width = M_ID,
        .matrix = (double *)f_id_shift_l,
    };
    Options opt_1 = {
        .factor = FACTOR_DEFAULT(f_id_shift_l, M_ID),
        .bias = 0.0,
        .filter = &filter_1,
    };

    Filter filter_2 = {
        .height = M_ID,
        .width = M_ID,
        .matrix = (double *)f_id_shift_r,
    };
    Options opt_2 = {
        .factor = FACTOR_DEFAULT(f_id_shift_r, M_ID),
        .bias = 0.0,
        .filter = &filter_2,
    };

    test_state->bmp_target_1 = conv_seq(test_state->bmp_source, opt_1);
    test_state->bmp_target_1 = conv_seq(test_state->bmp_target_1, opt_2);

    for (int y = 0; y < opt_1.filter->height; y++) {
        for (int x = 0; x < opt_1.filter->width; x++) {
            unsigned char r, g, b, r_tar, g_tar, b_tar;
            get_pixel_rgb(test_state->bmp_source, x, y, &r, &g, &b);
            get_pixel_rgb(test_state->bmp_target_1, x, y, &r_tar, &g_tar,
                          &b_tar);

            assert_almost_equal(r, r_tar);
            assert_almost_equal(g, g_tar);
            assert_almost_equal(b, b_tar);
        }
    }
}
