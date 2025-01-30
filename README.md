# Name : Aiden Mitchell
# SID : 1658859
# CCID : aiden2



# - - - - - - - - - - - D E S I G N - - - - - - - - - - - -
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - -




# - - - - - - - - - - - T E S T I N G - - - - - - - - - - -
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - -



# - - - - - - - - - - - S O U R C E S - - - - - - - - - - -
Random number generation  - https://www.youtube.com/watch?v=oW6iuFbwPDg&ab_channel=TheBuilder
Time of day - https://man7.org/linux/man-pages/man2/settimeofday.2.html
qsort - https://www.programiz.com/cpp-programming/library-function/cstdlib/qsort
Vector to array - https://stackoverflow.com/questions/2923272/how-to-convert-vector-to-array
barrier_wait - https://pubs.opengroup.org/onlinepubs/009696899/functions/pthread_barrier_wait.html
vector::begin - https://cplusplus.com/reference/vector/vector/begin/
vector of vector - https://www.geeksforgeeks.org/vector-of-vectors-in-c-stl-with-examples/
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


# - - - - - - - - - - - PLANNING - - - - - - - - - - - - -
Ok I think threadpool is not really needed. I can just assign manually before each stage. 
Threadpool may actually be faster but I don't think itll be enough to warrant me dealing with this mess.

Yeah I'm just gunna use create,join, and barriers. I don't see a reason to overcomplicate this.


For splitting the array I think I can do a recursive function. We'll see.
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
