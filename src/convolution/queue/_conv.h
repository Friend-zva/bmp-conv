#pragma once

#include "utils/_utils.h"

// count_ths - [count_readers, count_convs, count_writers]
int queue_mode(char **argv, Options opt, int count_ths[3]);
