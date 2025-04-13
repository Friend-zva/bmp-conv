#include <stdio.h>
#include <stdlib.h>

#include "utils.h"
#include "seq/_conv.h" // remove?!
#include "par/_conv.h" // remove?!
#include "options.h"
#include "seq.h"
#include "par.h"

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(non_null_conv_seq_test, setup, teardown),
        cmocka_unit_test_setup_teardown(non_null_conv_par_test, setup, teardown),
        cmocka_unit_test_setup_teardown(id_conv_seq_test, setup, teardown),
        cmocka_unit_test_setup_teardown(id_conv_par_test, setup, teardown),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
