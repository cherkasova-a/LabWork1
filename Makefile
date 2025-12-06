CXX = g++
CXXFLAGS = -std=c++17 -Wall -g
TARGET = lab1
TEST_TARGET = run_tests

SRC = main.cpp BMPImage.cpp
OBJ = $(SRC:.cpp=.o)

TEST_SRC = test_lab1.cpp BMPImage.cpp
TEST_OBJ = $(TEST_SRC:.cpp=.o)

all: $(TARGET) $(TEST_TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ)

$(TEST_TARGET): $(TEST_OBJ)
	$(CXX) $(CXXFLAGS) -o $(TEST_TARGET) $(TEST_OBJ) -lgtest -lpthread

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TEST_OBJ) $(TARGET) $(TEST_TARGET)

