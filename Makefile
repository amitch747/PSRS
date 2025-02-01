all: PSRS

PSRS: PSRS.cpp 
	g++ -std=c++20 -pthread -Ofast -o PSRS PSRS.cpp 
	
QS: QS.cpp 
	g++ -std=c++20 -pthread -O3 -o QS QS.cpp 


clean:
	rm -f PSRS QS