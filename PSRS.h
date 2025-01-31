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

pthread_barrier_t bar1;
pthread_barrier_t bar2;
pthread_barrier_t bar3;
pthread_barrier_t bar4;

pthread_mutex_t print_mutex;

struct phaseOneStruct {
    int* block; // sub array to be quick sorted
    size_t size;
    int w; 
    int p; 
    int pSq;
};

struct phaseThreeFourStruct {
    int* block;
    size_t size;
    int* pivots;
    int p; 
    int*** sharedPartitionSet;
    int** sharedPartitionSizes;
    int threadID;
};
