#include "PSRS.h"

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

void* phaseOne(void* arg)
{
    phaseOneStruct* p1 = static_cast<phaseOneStruct*>(arg);
    int* localBlock = p1->block;
    size_t blockSize = p1->size;
    int w = p1->w;
    int p = p1->p;
    int pSq = p1->pSq;
    //std::cout << "LocalBlock 1st element: " << *localBlock << ". List has size [" << blockSize << "]" << std::endl;
    qsort(localBlock, blockSize, sizeof(int), quickCompare);

    // for (size_t i = 0; i < blockSize; ++i) {
    //     //std::cout << localBlock[i] << " ";
    // }
    //std::cout << " of size [" << blockSize << "]" << std::endl;

    int* tempRegSamples = new int[p];

    //Each sorted list will "select data items at local indices 1, w+1, 2w+1, ... , (p-1)w+1"
    int k = 0;
    for (int i = 0; i<p; i++) {
        tempRegSamples[i] = localBlock[k];
        k+=w;
    }

    for (int i = 0; i < p; ++i) {
       // std::cout << tempRegSamples[i] << " ";
    }
   // std::cout << std::endl;
    //std::cout<< "Barrier wait" << std::endl;
    pthread_barrier_wait(&bar1);
    //std::cout<< "Barrier passed" << std::endl;
    return tempRegSamples;
}

void* phaseThreeFour(void* arg) {
    phaseThreeFourStruct* p34 = static_cast<phaseThreeFourStruct*>(arg);
    int* localBlock = p34->block;
    //int* backupBlock = p3->block;
    int* backupBlock = new int[p34->size]; // Allocate new space
    std::memcpy(backupBlock, p34->block, p34->size * sizeof(int)); // Copy data

    size_t blockSize = p34->size;
    int * pivots = p34->pivots;
    int p = p34->p;
    int*** sharedPartitionSet = p34->sharedPartitionSet;
    int** sharedPartitionSizes = p34->sharedPartitionSizes;
    int threadID = p34->threadID;



    int blockIndex = 0;
    //std::vector<std::vector<int>> blockPartitions;
    //int* partSizes = new int[p];

    for(int i = 0; i < p-1; i++) {
        int partSize = 0;
        while(*localBlock<=pivots[i]){
            partSize++;
           // std::cout << "*localBlock:" << *localBlock << " pivots[" << i << "]:" << pivots[i] << " partSize:" << partSize << std::endl;
            localBlock++;
            blockIndex++;
        }
        sharedPartitionSizes[threadID][i] = partSize;
    }
    //Get last partition
    int partSize = 0;
    while(blockIndex<blockSize){
        partSize++;
        //std::cout << "*localBlock:" << *localBlock << " last section" << " partSize:" << partSize << std::endl;
        localBlock++;
        blockIndex++;

    }
    sharedPartitionSizes[threadID][p-1] = partSize;


    
    sharedPartitionSet[threadID] = new int*[p];
    int* blockPtr = backupBlock; 
    for(int i = 0; i < p; i++) {
        sharedPartitionSet[threadID][i] = new int[sharedPartitionSizes[threadID][i]];
        //std::cout << "Thread[" << threadID<< "] " <<"Partition [" << i << "]:" << sharedPartitionSizes[threadID][i] << " elements worth of space" << std::endl << std::flush;

        int paritionIndex = 0;
        while(paritionIndex < sharedPartitionSizes[threadID][i]) {
            sharedPartitionSet[threadID][i][paritionIndex] = *blockPtr;
            blockPtr++;
            paritionIndex++;
            //std::cout << paritionIndex << std::endl;
        }
    }


    //std::cout << "Thread " << threadID << " before barrier" << std::endl;
    pthread_barrier_wait(&bar3);
    //std::cout << "Thread " << threadID << " after barrier" << std::endl;




    int sharedBlockSize = 0;
    for(int i = 0; i < p; i++)
    {
        sharedBlockSize += sharedPartitionSizes[i][threadID];
    }

    int* assignedPartitions = new int[sharedBlockSize]; // Each thread will have a list of partitions (also lists)
    for(int i = 0; i < p; i++) {
        int* partition_i = sharedPartitionSet[i][threadID];
        int partition_iSize = sharedPartitionSizes[i][threadID];
        std::memcpy(assignedPartitions, partition_i, partition_iSize* sizeof(int));
        assignedPartitions += partition_iSize;
    }
    assignedPartitions -= sharedBlockSize;

    qsort(assignedPartitions, sharedBlockSize, sizeof(int), quickCompare);

    //pthread_mutex_lock(&print_mutex);
    //std::cout << "Block size for thread[" << threadID << "]:" << sharedBlockSize << std::endl;

    //for(int k =0 ; k<sharedBlockSize; k++)
    //{
   //     std::cout << assignedPartitions[k] << " ";
    //}
     //std::cout <<std::endl;
    //pthread_mutex_unlock(&print_mutex);



    //std::cout << "Thread " << threadID << " before barrier" << std::endl;
    pthread_barrier_wait(&bar3);
    //std::cout << "Thread " << threadID << " after barrier" << std::endl;


    return assignedPartitions;
}




int main(int argc, char* argv[]) 
{
    pthread_mutex_init(&print_mutex, NULL);

    if(argc != 3) {  
        std::cerr << "Command requires the number of array elements, and the number of processors (threads)\n";
        return 1;
    }

    struct timeval tv0;
    struct timezone tz0;
    struct timeval tv1;
    struct timezone tz1;
    struct timeval tv5;
    struct timezone tz5;
    gettimeofday(&tv0, &tz0); // Get start time

    // Set up variables
    int n = atoi(argv[1]); // Size of list
    int p = atoi(argv[2]); // Number of processors (threads)
    int pSq = (int)pow(p,2);
    int rho = p/2; // ???
    int w = n/pSq; // ???
    int blockSize = n/p; // Blocksize for Phase1
    int remainder = n % p; // Used to fill up blocks up to n%p with an extra key


    // // //First need to create the array
    // int* a0 = new int[n];
    // std::random_device rd;
    // std::iota(a0, a0 +n, 0);
    // std::ranges::shuffle(a0, a0+n, rd);
    //std::cout << "Phase 0 array:" << std::endl;
    // for (int i = 0; i < n; ++i){
    //    // std::cout << a0[i] << " ";
    // }
    //std::cout << std::endl;
    // int* a0 = new int[n];
    // std::random_device rd;
    // std::mt19937 g(rd());

    // // Fill and shuffle in one step
 
    // std::shuffle(a0, a0 + n, g);

    // int* a0 = new int[n];
    // std::random_device rd;
    // std::mt19937 gen(rd());
    // std::uniform_int_distribution<> dis(0, n - 1);
    // for (int i = 0; i < n; ++i) {
    //     a0[i] = dis(gen); // Assuming a0 is your array
    // }
    // for (int i = 0; i < n; ++i){
    //     std::cout << a0[i] << " ";
    // // }

    int* a0 = new int[n];
    std::vector<int> numbers(n);
    std::iota(numbers.begin(), numbers.end(), 0); // Fill with 0, 1, ..., 35
    std::random_device rd;
    //std::mt19937 gen(rd());
    std::minstd_rand0 gen(rd());


    //std::cout << "Setup execution Pre-shuffle " << sec01 << " seconds and "<< msec01  << " microseconds" << std::endl;
    std::shuffle(numbers.begin(), numbers.end(), gen);

    std::copy(numbers.begin(), numbers.end(), a0);

    // for (int i = 0; i < 10; ++i) {
    //     std::cout << numbers[i] << " ";
    // }

    // Print the array
    // for (int i = 0; i < n; ++i) {
    //     std::cout << a0[i] << " ";
    // }

    gettimeofday(&tv1, &tz1);
    long sec0 = tv1.tv_sec - tv0.tv_sec;
    long msec0 = tv1.tv_usec - tv0.tv_usec;
    if (msec0 < 0) {   
        //Case where msec is negative, need to fix up
        sec0= sec0 -1;
        msec0 = msec0+ 1000000;
    }
    std::cout << "Setup execution time: " << sec0 << " seconds and "<< msec0  << " microseconds" << std::endl;
    //sleep(100);



    // Needed info for Phase 1
    int** blocks = new int*[p];
    size_t * blockSizes = new size_t[p]; // may as well do this now, instead of in the parallel portions

    pthread_setconcurrency(p);
    pthread_t threads[p];
    pthread_barrier_init(&bar1, NULL, p);



    
    /*
    PHASE 1: Sort Local Data
    - This will involve giving each thread a subsection of v0 as a function input
    - Function should output (for each thread) a sorted block, and the local regular samples
    */
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



    
    struct timeval tv2a;
    struct timezone tz2a;
    struct timeval tv2b;
    struct timezone tz2b;
    gettimeofday(&tv2a, &tz2a); // Get start time
    
    // PHASE 2: Find Pivots then Partition
    int* RegSamples = new int[pSq];

    for (int i = 0; i < p; i++) {
        int* tempRegSamp;
        if (pthread_join(threads[i], (void**)&tempRegSamp) != 0) {
            perror("Failed to JOIN a thread after phaseOne job");
        }
        //std::cout << "Regular sample [" << i << "] ";
        for (int y = 0; y < p; ++y) {
            //std::cout << tempRegSamp[y] << " ";
            RegSamples[i*p+y] = tempRegSamp[y]; //LMFAO HARD CODE IN 3??????????
        }
        //std::cout << std::endl;

    }
    // for (int y = 0; y < pSq; ++y) {
    //         std::cout << RegSamples[y] << " ";
    //     }
    //std::cout << std::endl;
    // for (int i = 0; i < p; ++i) {
    //     std::cout << "Sorted Block" << i + 1 << " of size:[" << blockSizes[i] << "]: ";
    //     for (size_t j = 0; j < blockSizes[i]; ++j) {
    //         std::cout << blocks[i][j] << " ";
    //     }
    // std::cout << std::endl;
    // }


    // std::cout << "RegSamples:\n" << "";
    // for (int y = 0; y < pSq; ++y) {
    //         std::cout << RegSamples[y] << " ";
    //     }
    //     std::cout << std::endl;


    qsort(RegSamples, pSq, sizeof(int), quickCompare);

    int* pivots = new int[p];
    int piv = p;
    int mult = 1;
    for (int i = 0; i<p-1; i++) {
        int index = ((piv*mult)+rho-1);

        pivots[i] = RegSamples[index];
        mult++;
    }
    // std::cout << "Pivots:\n" << "";
    // for (int y = 0; y < p-1; ++y) {
    //         std::cout << pivots[y] << " ";
    //     }
    // std::cout << std::endl;

    gettimeofday(&tv2b, &tz2b);
    long sec2 = tv2b.tv_sec - tv2a.tv_sec;
    long msec2 = tv2b.tv_usec - tv2a.tv_usec;
    if (msec2 < 0) {   
        //Case where msec is negative, need to fix up
        sec2 = sec2 -1;
        msec2 = msec2 + 1000000;
    }
    std::cout << "Phases 2 execution time " << sec2 << " seconds and "<< msec2  << " microseconds" << std::endl;






    struct timeval tv3;
    struct timezone tz3;
    struct timeval tv4;
    struct timezone tz4;
    gettimeofday(&tv3, &tz3); // Get start time


    //PHASE 3: Exchange Partitions
    int*** sharedPartitionSet = new int**[p]; // Global memory space for threads to exchange paritions
    int** sharedPartitionSizes = new int*[p]; 
    for(int s = 0; s<p ; s++) {sharedPartitionSizes[s] = new int[p];}

    pthread_barrier_destroy(&bar1);
    pthread_barrier_init(&bar3, NULL, p);
    for (int i = 0; i < p; i++) {
        phaseThreeFourStruct* p34 = new phaseThreeFourStruct(blocks[i], blockSizes[i], pivots, p, sharedPartitionSet, sharedPartitionSizes, i); // Data available for each thread
        if (pthread_create(&threads[i], NULL, &phaseThreeFour, p34) != 0) {
            perror("Failed to CREATE a thread for phaseThreeFour job");
        }
    }

    //PHASE 4: Merge Partitions
    std::vector<int> outputArray(n);
    int offset = 0;

    for (int i = 0; i < p; i++) {
        int* partition;
        if (pthread_join(threads[i], (void**)&partition) != 0) {
            perror("Failed to JOIN a thread after phaseThreeFour job");
        }
        int sharedBlockSize = 0;
        for (int j = 0; j < p; j++) {sharedBlockSize += sharedPartitionSizes[j][i];}

        std::copy(partition, partition + sharedBlockSize, outputArray.begin() + offset);
        offset += sharedBlockSize;
        delete[] partition;
    }   



    gettimeofday(&tv4, &tz4);
    long sec34 = tv4.tv_sec - tv3.tv_sec;
    long msec34 = tv4.tv_usec - tv3.tv_usec;
    if (msec34 < 0) {   
        //Case where msec is negative, need to fix up
        sec34 = sec34 -1;
        msec34 = msec34 + 1000000;
    }
    std::cout << "Phases 3/4 execution time " << sec34 << " seconds and "<< msec34  << " microseconds" << std::endl;

    pthread_barrier_destroy(&bar3);
    //sleep(2);
    // std::cout << "Final Output Array: ";
    // for (int i = 0; i < n; ++i) {
    //     std::cout << outputArray[i] << " ";
    // }
    // std::cout << std::endl;


    // Get end time
    gettimeofday(&tv5, &tz5);
    long sec = tv5.tv_sec - tv0.tv_sec;
    long msec = tv5.tv_usec - tv0.tv_usec;
    if (msec < 0) {   
        //Case where msec is negative, need to fix up
        sec = sec -1;
        msec = msec + 1000000;
    }
    std::cout << "Total execution time: " << sec << " seconds and "<< msec  << " microseconds" << std::endl;
    return 0;
}