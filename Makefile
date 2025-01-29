all: PSRS

PSRS: PSRS.cc
	g++ -std=c++20 -o PSRS PSRS.cc

clean:
	rm -f PSRS