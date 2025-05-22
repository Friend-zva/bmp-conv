#include "utils.h"

#include <stdio.h>
#include <stdlib.h>

#include "config.h"

int setup(void **state) {
    TestState *test_state = (TestState *)malloc(sizeof(TestState));
    if (test_state == NULL) {
        return -1;
    }

    test_state->bmp_source = bopen(BMP_PATH);
    if (test_state->bmp_source == NULL) {
        free(test_state);
        return -1;
    }

    test_state->bmp_target_1 = NULL;
    test_state->bmp_target_2 = NULL;

    *state = test_state;
    return 0;
}

int teardown(void **state) {
    TestState *test_state = (TestState *)*state;

    if (test_state->bmp_source != NULL) {
        bclose(test_state->bmp_source);
    }
    if (test_state->bmp_target_1 != NULL) {
        bclose(test_state->bmp_target_1);
    }
    if (test_state->bmp_target_2 != NULL) {
        bclose(test_state->bmp_target_2);
    }

    free(test_state);
    return 0;
}

void assert_almost_equal(char a, char b) {
    assert_true(abs((int)a - (int)b) <= 1);
}
