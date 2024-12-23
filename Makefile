# Makefile

CXX = g++
CXXFLAGS = -std=c++14 -pthread -Wall -Wextra -g -fprofile-arcs -ftest-coverage # -g for valgrind , -fprofile-arcs -ftest-coverage for gcov (code coverage)

SERVER_SRCS = main.cpp Server.cpp Graph.cpp PrimAlgorithm.cpp KruskalAlgorithm.cpp MSTFactory.cpp Measurements.cpp DisjointSet.cpp ThreadPool.cpp ActiveObject.cpp
SERVER_OBJS = $(SERVER_SRCS:.cpp=.o)

CLIENT_SRCS = client.cpp
CLIENT_OBJS = $(CLIENT_SRCS:.cpp=.o)

DEPS = Edge.h Graph.h MSTAlgorithm.h PrimAlgorithm.h KruskalAlgorithm.h MSTFactory.h Measurements.h DisjointSet.h ThreadPool.h Server.h ActiveObject.h

all: server client

server: $(SERVER_OBJS)
	$(CXX) $(CXXFLAGS) -o server $(SERVER_OBJS)

client: $(CLIENT_OBJS)
	$(CXX) $(CXXFLAGS) -o client $(CLIENT_OBJS)

%.o: %.cpp $(DEPS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o server client

.PHONY: all clean
