CXX = g++
CXXFLAGS = -std=c++20 -Wall -I.

SRCS = main.cpp \
       automata/NFA.cpp \
       automata/DFA.cpp \
       automata/NFADeterminizer.cpp \
       automata/DFAMinimizer.cpp \
       regex/RegexScanner.cpp \
       regex/RegexParser.cpp \
       regex/RegexPreprocessor.cpp \
       regex/RegexASTToNFA.cpp \
       utils/AutomataVisualize.cpp \
       utils/RegexASTVisualizer.cpp

OBJS = $(SRCS:.cpp=.o)
TARGET = main.exe

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
	rm -rf graphviz/ images/

.PHONY: all clean