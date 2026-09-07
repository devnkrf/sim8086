CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra
#SRC = haversine_generator.cpp
#TARGET = bin/haversine_generator
SRC = json_parser.cpp
TARGET = json_parser

$(TARGET): $(SRC)
	#mkdir -p bin
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

clean:
	#rm -rf bin
	rm $(TARGET)

