# Variables
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -Iinclude
SRC = src/MemoryManager.cpp main.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = memsim

# Default rule to build the project
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ)

# Rule to clean up files
clean:
	rm -f $(OBJ) $(TARGET)