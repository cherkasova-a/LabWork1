CXX = g++
CXXFLAGS = -std=c++17 -Wall -g

SRCS = main.cpp BMPImage.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = lab1

TEST_SRCS = test_lab1.cpp BMPImage.cpp
TEST_OBJS = $(TEST_SRCS:.cpp=.o)
TEST_TARGET = run_tests

all: $(TARGET) $(TEST_TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

$(TEST_TARGET): $(TEST_OBJS)
	$(CXX) $(CXXFLAGS) -o $(TEST_TARGET) $(TEST_OBJS) -lgtest -lpthread

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f $(OBJS) $(TARGET) $(TEST_OBJS) $(TEST_TARGET)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

