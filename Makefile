all: PSRS

PSRS: PSRS.cpp
	g++ -std=c++20 -o PSRS PSRS.cpp

clean:
	rm -f PSRS