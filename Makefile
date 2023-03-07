CXX = g++
CXXFLAGS = -Wall -Wextra -pedantic -std=c++20

SRCS = ipkcpc.cpp
OBJS = $(SRCS:.cpp=.o)

TARGET = ipkcpc

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

runtcp: all
	./$(TARGET) -h 0.0.0.0 -p 2023 -m tcp

runudp: all
	./$(TARGET) -h 0.0.0.0 -p 2023 -m udp

clean:
	$(RM) $(OBJS) $(TARGET)
