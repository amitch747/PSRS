#pragma once
#include <cstdlib>
#include <iostream>
#include <stdlib.h>

#include <ranges>
#include <vector>
#include <random>
#include <algorithm>
#include <sys/time.h>
#include <pthread.h>

#include <cmath>
struct phaseOneStruct {
    int* block; // sub array to be quick sorted
    size_t size;
};