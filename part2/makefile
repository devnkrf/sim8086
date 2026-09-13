CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra

SRC_GEN = haversine_generator.cpp
TARGET_GEN = bin/haversine_generator

SRC_SIM = simple_haversine.cpp
TARGET_SIM = bin/simple_haversine

all: $(TARGET_GEN) $(TARGET_SIM)

$(TARGET_GEN): $(SRC_GEN)
	mkdir -p bin
	$(CXX) $(CXXFLAGS) -o $(TARGET_GEN) $(SRC_GEN)

$(TARGET_SIM): $(SRC_SIM)
	mkdir -p bin
	$(CXX) $(CXXFLAGS) -o $(TARGET_SIM) $(SRC_SIM)

clean:
	rm -rf bin

