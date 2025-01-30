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
    // Set up variables
    int n = atoi(argv[1]); // Size of list
    int p = atoi(argv[2]); // Number of processors (threads)
    int rho = p/2; // ???
    int w = n/(pow(p,2)); // ???
    int blockSize = n/p; // Blocksize for Phase1


    //First need to create the array
    std::vector<int> v0(atoi(argv[1]));
    std::random_device rd;
    std::iota(v0.begin(), v0.end(), 0);
    std::ranges::shuffle(v0,rd);
    std::cout << "Phase 0 list:" << std::endl;
    for (auto i : v0){
        //std::cout << i << " ";
    }

    int* a0 = &v0[0]; // Convert to array ;



    

    //Then the threads

    // Plan for tmrw: 
    // Use paper to set up all possible variables I'll need.
    // Figure out Pthreads. Creating, assigning jobs, barrier syncs, and joining at the end. 

    /*
    PHASE 1: Sort Local Data

    - This will involve giving each thread a subsection of v0 as a function input
    - Function should output (for each thread) a sorted block, and the local regular samples
    */


    // Getting samples is a for loop where you append a vector. Loop stops at p^2

    /*
    PHASE 2: Find Pivots then Partition

    - Thread 1 (or I suppose the first thread to grab the job) re-combines the local regular samples and sorts them again
    - Output pivots
    */

    /*
    PHASE 3: Exchange Partitions

    - Use pivots to split each sorted local block into partitions
    */

    /*
    PHASE 4: Merge Partitions

    - Partitions 1-n are given to respective threads
    - Partitions are merged
    - Finally, merged partitions are mergred with eachother to form sorted list
    */


    // Get end time
    gettimeofday(&tv2, &tz2);
    long sec = tv2.tv_sec - tv1.tv_sec;
    long msec = tv2.tv_usec - tv1.tv_usec;
    //Case where msec is negative, need to fix up
    if (msec < 0) {
        sec = sec -1;
        msec = msec + 1000000;
    }
    std::cout << "\nPhase 5 list:" << std::endl;
    for (auto i : v0){
        std::cout << i << " ";
    }
    std::cout << "\nExecution time: " << sec << " seconds and "<< msec  << " microseconds" << std::endl;
    return 0;
}