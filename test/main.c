#include <stdio.h>
#include <stdlib.h>

#include "utils.h"
#include "seq/_conv.h" // remove?!
#include "par/_conv.h" // remove?!
#include "options.h"
#include "seq.h"
#include "par.h"

// replace?!
static void eq_conv_seq_pthread_test(void **state) {
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

    test_state->bmp_seq = conv_seq(test_state->bmp_source, opt);
    test_state->bmp_par = conv_par(test_state->bmp_source, opt);

    for (int y = 0; y < opt.filter->height; y++)
        for (int x = 0; x < opt.filter->width; x++) {
            unsigned char r_seq, g_seq, b_seq, r_par, g_par, b_par;
            get_pixel_rgb(test_state->bmp_seq, x, y, &r_seq, &g_seq, &b_seq);
            get_pixel_rgb(test_state->bmp_par, x, y, &r_par, &g_par, &b_par);

            assert_int_equal(r_seq, r_par);
            assert_int_equal(g_seq, g_par);
            assert_int_equal(b_seq, b_par);
        }
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(non_null_conv_seq_test, setup, teardown),
        cmocka_unit_test_setup_teardown(non_null_conv_par_test, setup, teardown),
        cmocka_unit_test_setup_teardown(id_conv_seq_test, setup, teardown),
        cmocka_unit_test_setup_teardown(id_conv_par_test, setup, teardown),
        cmocka_unit_test_setup_teardown(eq_conv_seq_pthread_test, setup, teardown),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
