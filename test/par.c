#include "par.h"

#include "options.h"
#include "par/_conv.h"
#include "utils.h"

void non_null_conv_par_test(void **state) {
    TestState *test_state = (TestState *)*state;

    Filter filter = {
        .height = M_ONE,
        .width = M_ONE,
        .matrix = (double *)f_one,
    };
    Options opt = {
        .factor = 1.0,
        .bias = 0.0,
        .filter = &filter,
    };
    test_state->bmp_target_1 = conv_par(test_state->bmp_source, opt, COLUMN, 8);

    assert_non_null(test_state->bmp_target_1);
}

void id_conv_par_test(void **state) {
    TestState *test_state = (TestState *)*state;

    Filter filter = {
        .height = M_ID,
        .width = M_ID,
        .matrix = (double *)f_id,
    };
    Options opt = {
        .factor = 1.0,
        .bias = 0.0,
        .filter = &filter,
    };
    test_state->bmp_target_1 = conv_par(test_state->bmp_source, opt, COLUMN, 8);

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

void eq_row_column_bl_conv_par_test(void **state) {
    TestState *test_state = (TestState *)*state;

    Filter filter = {
        .height = M_BL,
        .width = M_BL,
        .matrix = (double *)f_blur_lite,
    };
    Options opt_1 = {
        .factor = 1.0 / sum_elem_matrix((double *)f_blur_lite, M_BL),
        .bias = 0.0,
        .filter = &filter,
    };
    test_state->bmp_target_1 = conv_par(test_state->bmp_source, opt_1, ROW, 8);

    Options opt_2 = {
        .factor = 1.0 / sum_elem_matrix((double *)f_blur_lite, M_BL),
        .bias = 0.0,
        .filter = &filter,
    };
    test_state->bmp_target_2 =
        conv_par(test_state->bmp_source, opt_2, COLUMN, 8);

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

void eq_row_pixel_bm_conv_par_test(void **state) {
    TestState *test_state = (TestState *)*state;

    Filter filter = {
        .height = M_BM,
        .width = M_BM,
        .matrix = (double *)f_blur_medium,
    };
    Options opt_1 = {
        .factor = 1.0 / sum_elem_matrix((double *)f_blur_medium, M_BM),
        .bias = 0.0,
        .filter = &filter,
    };
    test_state->bmp_target_1 = conv_par(test_state->bmp_source, opt_1, ROW, 8);

    Options opt_2 = {
        .factor = 1.0 / sum_elem_matrix((double *)f_blur_medium, M_BM),
        .bias = 0.0,
        .filter = &filter,
    };
    test_state->bmp_target_2 =
        conv_par(test_state->bmp_source, opt_2, PIXEL, 8);

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
