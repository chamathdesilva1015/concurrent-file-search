CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2
TARGET = file_search
GENERATOR = generate_test_data

SRCS = src/main.cpp src/file_searcher.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET) $(GENERATOR)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

$(GENERATOR): generate_test_data.cpp
	$(CXX) $(CXXFLAGS) -o $(GENERATOR) generate_test_data.cpp

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(GENERATOR) $(OBJS)
	rm -rf large_test_data

.PHONY: all clean