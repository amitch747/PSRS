#include <cstdlib>
#include <iostream>
#include <stdlib.h>

#include <ranges>
#include <vector>
#include <random>
#include <algorithm>

int main(int argc, char* argv[]) 
{
    if(argc != 3) 
    {  
        std::cerr << "ERROR\n";
        std::cerr << "Input consists of the .exe, the number of array elements, and the number of processors (threads)\n";
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
    return 0;
}