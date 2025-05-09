#include <stdio.h>
#include <stdlib.h>

#include "options.h"
#include "par.h"
#include "par/_conv.h"  // remove?!
#include "seq.h"
#include "seq/_conv.h"  // remove?!
#include "utils.h"

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
        .mode = ROW,
    };
    test_state->bmp_target_1 = conv_seq(test_state->bmp_source, opt);
    test_state->bmp_target_2 = conv_par(test_state->bmp_source, opt);

    for (int y = 0; y < opt.filter->height; y++) {
        for (int x = 0; x < opt.filter->width; x++) {
            unsigned char r_seq, g_seq, b_seq, r_par, g_par, b_par;
            get_pixel_rgb(test_state->bmp_target_1, x, y, &r_seq, &g_seq,
                          &b_seq);
            get_pixel_rgb(test_state->bmp_target_2, x, y, &r_par, &g_par,
                          &b_par);

            assert_true(r_seq == r_par);
            assert_true(g_seq == g_par);
            assert_true(b_seq == b_par);
        }
    }
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(non_null_conv_seq_test, setup,
                                        teardown),
        cmocka_unit_test_setup_teardown(non_null_conv_par_test, setup,
                                        teardown),
        cmocka_unit_test_setup_teardown(id_conv_seq_test, setup, teardown),
        cmocka_unit_test_setup_teardown(id_conv_par_test, setup, teardown),
        cmocka_unit_test_setup_teardown(eq_row_column_bl_conv_seq_test, setup,
                                        teardown),
        cmocka_unit_test_setup_teardown(eq_row_column_bm_conv_par_test, setup,
                                        teardown),
        cmocka_unit_test_setup_teardown(eq_seq_par_bl_conv_test, setup,
                                        teardown),

    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
