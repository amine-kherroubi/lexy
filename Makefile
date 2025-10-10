CXX = g++
CXXFLAGS = -std=c++11 -Wall -I.

SRCS = main.cpp automata/NFA.cpp automata/DFA.cpp automata/RegexPreprocessor.cpp automata/RegexToNFA.cpp automata/NFADeterminizer.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = test

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean