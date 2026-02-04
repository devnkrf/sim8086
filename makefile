CXX = g++
CXXFLAGS = -Wall -O2
TARGET = bin/sim86
SRC = sim86.cpp
LIB = ./lib_sim86.so

all: $(TARGET)

$(TARGET): $(SRC)
	mkdir -p bin
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC) $(LIB)

clean:
	rm -rf bin
