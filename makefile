CXX = g++
CXXFLAGS = -Wall -O2
TARGET = bin/sim86
SRC = sim86.cpp
LIB = -L. -lsim86

all: $(TARGET)

$(TARGET): $(SRC)
	mkdir -p bin
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC) $(LIB)

clean:
	rm -rf bin
