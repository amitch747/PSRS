#include <cstdlib>
#include <iostream>
#include <stdlib.h>
#include<unistd.h>

#include <ranges>
#include <vector>
#include <random>
#include <algorithm>
#include <sys/time.h>
#include <pthread.h>

#include <cmath>

#include <cstdint>
#include <cstring>
 

int quickCompare(const void* x, const void* y)
{
    const int* a = (int*) x;
	const int* b = (int*) y;
	if (*a > *b) return 1;
	else if (*a < *b) return -1;
	return 0;
}


 int main(int argc, char* argv[]) 
{
 
    if(argc != 2) {  
        std::cerr << "Command requires the number of array elements\n";
        return 1;
    }

    struct timeval tv1;
    struct timezone tz1;
    struct timeval tv2;
    struct timezone tz2;
    gettimeofday(&tv1, &tz1); // Get start time

    // // Set up variables
     int n = atoi(argv[1]); // Size of list
    // int* a0 = new int[n];

    // std::random_device rd;
    // std::iota(a0, a0 +n, 0);
    // std::ranges::shuffle(a0, a0+n, rd);


    int* a0 = new int[n];
    std::vector<int> numbers(n);
    std::iota(numbers.begin(), numbers.end(), 0); // Fill with 0, 1, ..., 35
    std::random_device rd;
    //std::mt19937 gen(rd());
    std::minstd_rand0 gen(rd());


    //std::cout << "Setup execution Pre-shuffle " << sec01 << " seconds and "<< msec01  << " microseconds" << std::endl;
    //std::shuffle(numbers.begin(), numbers.end(), gen);
    for (int i = 0; i < n; ++i) {
        std::swap(numbers[i], numbers[gen() % n]);
     }
    std::copy(numbers.begin(), numbers.end(), a0);



    qsort(a0, n, sizeof(int), quickCompare);

    gettimeofday(&tv2, &tz2);
    long sec = tv2.tv_sec - tv1.tv_sec;
    long msec = tv2.tv_usec - tv1.tv_usec;
    if (msec < 0) {   
        //Case where msec is negative, need to fix up
        sec = sec -1;
        msec = msec + 1000000;
    }
    std::cout << "Total execution time: " << sec << " seconds and "<< msec  << " microseconds" << std::endl;
    return 0;

}