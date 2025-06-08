#pragma once

#include "utils/utils.h"

// counts_thread = [number of readers, number of workers, number of writers]
int queue_mode(char **argv, Options opt, int counts_thread[3]);
