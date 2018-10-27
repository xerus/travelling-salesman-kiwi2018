CXXFLAGS=-std=gnu++11 -O3 -Wall -Wextra

all: tsm

tsm: cpp/main.cpp
	g++ cpp/main.cpp $(CXXFLAGS) -o tsm

test: tsm
	time -f "seconds: %e \nkbytes: %M" sh ./test.sh

clean:
	rm -rf tsm
