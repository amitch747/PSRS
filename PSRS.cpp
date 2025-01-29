#include <cstdlib>
#include <iostream>
#include <stdlib.h>

#include <ranges>
#include <vector>
#include <random>
#include <algorithm>
#include <sys/time.h>



int main(int argc, char* argv[]) 
{
    struct timeval tv1;
    struct timezone tz1;
    struct timeval tv2;
    struct timezone tz2;
    // Get start time
    gettimeofday(&tv1, &tz1);

    if(argc != 3) 
    {  
        std::cerr << "ERROR\n";
        std::cerr << "Command requires the number of array elements, and the number of processors (threads)\n";
        return 1;
    }


    //First need to create the array
    std::vector<int> v(atoi(argv[1]));
    std::random_device rd;
    std::iota(v.begin(), v.end(), 0);
    std::ranges::shuffle(v,rd);
    for (auto i : v)
    {
        std::cout << i << " ";
    }

    //Then the threads

 


    // Phase One: Sort Local Data

    // Phase Two: Find Pivots then Partition

    // Phase 3: Exchange Partitions

    // Phase 4: Merge Partitions


    // Get end time
    gettimeofday(&tv2, &tz2);
    std::cout << "Execution time: " << tv2.tv_sec - tv1.tv_sec << " seconds and "<< tv2.tv_usec - tv1.tv_usec  << " microseconds" << std::endl;
    return 0;
}