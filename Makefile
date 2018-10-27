CXXFLAGS=-std=gnu++11 -O3 -Wall -Wextra

all: tsm

tsm: cpp/main.cpp
	g++ cpp/main.cpp $(CXXFLAGS) -o tsm

test: tsm
	sh ./test.sh
