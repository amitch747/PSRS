#include <cstdlib>
#include <iostream>

int main(int argc, char* argv[]) 
{
    if(argc != 3) 
    {  
        std::cerr << "ERROR\n";
        std::cerr << "Input consists of the .exe, the number of array elements, and the number of processors (threads)\n";
        return 1;
    }
    // Phase One: Sort Local Data

    // Phase Two: Find Pivots then Partition

    // Phase 3: Exchange Partitions

    // Phase 4: Merge Partitions
    return 0;
}