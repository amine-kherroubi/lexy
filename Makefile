CXX = g++
CXXFLAGS = -std=c++17 -Wall -I.

SRCS = main.cpp automata/NFA.cpp automata/DFA.cpp automata/RegexPreprocessor.cpp automata/RegexToNFA.cpp automata/NFADeterminizer.cpp automata/DFAMinimizer.cpp automata/Visualizer.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = main.exe

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) *.dot *.png *.pdf *.svg

.PHONY: all clean