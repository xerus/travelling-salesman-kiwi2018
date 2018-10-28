CXXFLAGS=-std=c++14 -O3 -Wall -Wextra -DDEBUG_OUTPUT

all: tsm

tsm: cpp/main.cpp
	g++ cpp/main.cpp $(CXXFLAGS) -o tsm

test: tsm
	time -f "seconds: %e \nkbytes: %M" sh ./test.sh

clean:
	rm -rf tsm
