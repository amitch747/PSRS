#include "PSRS.h"

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

int quickCompare(const void* x, const void* y)
{
    const int* a = (int*) x;
	const int* b = (int*) y;

	if (*a > *b) return 1;
	else if (*a < *b) return -1;
	return 0;
}

void* phaseOne(void* arg)
{
    phaseOneStruct* p1 = static_cast<phaseOneStruct*>(arg);
    int* localBlock = p1->block;
    size_t blockSize = p1->size;
    int w = p1->w;
    int p = p1->p;
    int pSq = p1->pSq;
    std::cout << "LocalBlock 1st element: " << *localBlock << ". List has size [" << blockSize << "]" << std::endl;
    qsort(localBlock, blockSize, sizeof(int), quickCompare);

    for (size_t i = 0; i < blockSize; ++i) {
        std::cout << localBlock[i] << " ";
    }
    std::cout << " of size [" << blockSize << "]" << std::endl;

    int* tempRegSamples = new int[p];

    //Each sorted list will "select data items at local indices 1, w+1, 2w+1, ... , (p-1)w+1"
    int k = 0;
    for (int i = 0; i<p; i++) {
        tempRegSamples[i] = localBlock[k];
        k+=w;
    }

    for (int i = 0; i < p; ++i) {
        std::cout << tempRegSamples[i] << " ";
    }
    std::cout << std::endl;
    std::cout<< "Barrier wait" << std::endl;
    pthread_barrier_wait(&bar1);
    std::cout<< "Barrier passed" << std::endl;
    return tempRegSamples;
}




int main(int argc, char* argv[]) 
{
    if(argc != 3) {  
        std::cerr << "Command requires the number of array elements, and the number of processors (threads)\n";
        return 1;
    }

    struct timeval tv1;
    struct timezone tz1;
    struct timeval tv2;
    struct timezone tz2;
    gettimeofday(&tv1, &tz1); // Get start time

    // Set up variables
    int n = atoi(argv[1]); // Size of list
    int p = atoi(argv[2]); // Number of processors (threads)
    int pSq = (int)pow(p,2);
    int rho = p/2; // ???
    int w = n/pSq; // ???
    int blockSize = n/p; // Blocksize for Phase1
    int remainder = n % p; // Used to fill up blocks up to n%p with an extra key


    //First need to create the array
    int* a0 = new int[n];
    std::random_device rd;
    std::iota(a0, a0 +n, 0);
    std::ranges::shuffle(a0, a0+n, rd);
    std::cout << "Phase 0 array:" << std::endl;
    for (int i = 0; i < n; ++i){
       // std::cout << a0[i] << " ";
    }
    std::cout << std::endl;

    // Needed info for Phase 1
    int** blocks = new int*[p];
    size_t * blockSizes = new size_t[p]; // may as well do this now, instead of in the parallel portions
    pthread_t threads[p];
    pthread_barrier_init(&bar1, NULL, p);

    int key = 0;
    for (int i = 0; i < p; i++) {

      
        int currentPartSize = blockSize + (i < remainder ? 1 : 0);  // We deal with remaider by giving an extra key to each block under the size of the reaminder. 
        blockSizes[i] = currentPartSize; // Set size of block
        blocks[i] = new int[currentPartSize]; // Create block on heap
        for (int j = 0; j < currentPartSize; ++j) {blocks[i][j] = a0[key++];}

        phaseOneStruct* p1 = new phaseOneStruct(blocks[i], blockSizes[i], w, p ,pSq); // Data available for each thread

        if (pthread_create(&threads[i], NULL, &phaseOne, p1) != 0) {
            perror("Failed to CREATE a thread for phaseOne job");
        }
    }

       int* RegSamples = new int[p];


    for (int i = 0; i < p; i++) {
        int* tempRegSamp;
        if (pthread_join(threads[i], (void**)&tempRegSamp) != 0) {
            perror("Failed to JOIN a thread after phaseOne job");
        }
        std::cout << "Regular sample [" << "i" << "] ";
        for (int y = 0; y < p; ++y) {
            std::cout << tempRegSamp[y] << " ";
            RegSamples[i*3+y] = tempRegSamp[y];
        }
        std::cout << std::endl;

    }


    for (int y = 0; y < pSq; ++y) {
            std::cout << RegSamples[y] << " ";
        }
        std::cout << std::endl;

    

    for (int i = 0; i < p; ++i) {
        std::cout << "Sorted Block" << i + 1 << " of size:[" << blockSizes[i] << "]: ";
        for (size_t j = 0; j < blockSizes[i]; ++j) {
            std::cout << blocks[i][j] << " ";
        }
        std::cout << std::endl;
    }

    pthread_barrier_destroy(&bar1);



    qsort(RegSamples, pSq, sizeof(int), quickCompare);

    for (int y = 0; y < pSq; ++y) {
            std::cout << RegSamples[y] << " ";
        }
        std::cout << std::endl;

    // delete[] a0;
    // for (int i = 0; i < p; i++) {
    //     delete[] blocks[i];
    // }
    //delete[] blocks;
    delete[] blockSizes;
    //delete[] RegSamples;
    int* pivots = new int[p];
    int piv = p;
    int mult = 1;
    for (int i = 0; i<p-1; i++) {
        int index = ((piv*mult)+rho-1);

        pivots[i] = RegSamples[index];
        mult++;
    }

    for (int y = 0; y < p-1; ++y) {
            std::cout << pivots[y] << " ";
        }
        std::cout << std::endl;

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
    for (int i = 0; i < n; ++i){
       // std::cout << a0[i] << " ";
    }
    std::cout << std::endl;
    std::cout << "\nExecution time: " << sec << " seconds and "<< msec  << " microseconds" << std::endl;
    return 0;
}