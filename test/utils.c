#include <stdio.h>
#include <stdlib.h>

#include "utils.h"
#include "config.h"
#include "options.h"

int setup(void **state) {
    TestState *test_state = (TestState *) malloc(sizeof(TestState));
    if (test_state == NULL)
        return -1;

    test_state->bmp_source = bopen(BMP_PATH);
    if (test_state->bmp_source == NULL) {
        free(test_state);
        return -1;
    }

    test_state->bmp_seq = NULL;

    *state = test_state;
    return 0;
}

int teardown(void **state) {
    TestState *test_state = (TestState *) *state;

    if (test_state->bmp_source != NULL)
        bclose(test_state->bmp_source);

    if (test_state->bmp_seq != NULL)
        bclose(test_state->bmp_seq);

    free(test_state);
    return 0;
}
