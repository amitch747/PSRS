all: PSRS

PSRS: PSRS.cpp threadpool.o
	g++ -std=c++20 -pthread - Wall -Werror -o PSRS PSRS.cpp threadpool.o

threadpool.o: threadpool.cpp threadpool.h
	g++ -std=c++20 -c threadpool.cpp



clean:
	rm -f PSRS