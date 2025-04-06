#include <stdio.h>
#include <stdlib.h>

#include "utils.h"
#include "seq.h"

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup_teardown(non_null_conv_seq_test, setup, teardown),
        cmocka_unit_test_setup_teardown(id_conv_seq_test, setup, teardown),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
