CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -Iinclude

# Source files
SRCS = main.cpp \
       src/automata/nfa.cpp \
       src/automata/dfa.cpp \
       src/automata/nfa_determinizer.cpp \
       src/automata/dfa_minimizer.cpp \
       src/automata/thompson_construction.cpp \
       src/regex/scanner.cpp \
       src/regex/parser.cpp \
       src/regex/preprocessor.cpp \
       src/regex/ast_to_nfa.cpp \
       src/visualization/automata_visualizer.cpp \
       src/visualization/regex_ast_visualizer.cpp \
	   src/user_specifications/user_spec_scanner.cpp \
	   src/user_specifications/user_spec_parser.cpp \
	   src/code_generation/code_generator.cpp

# Object files (in build directory)
BUILD_DIR = build
OBJS = $(SRCS:%.cpp=$(BUILD_DIR)/%.o)

# Target executable
TARGET = scanner_generator.exe

# Default target
all: $(TARGET)

# Link executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^
	@echo "Build complete: $(TARGET)"

# Compile source files
$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR) $(TARGET)
	rm -rf graphviz/ images/

# Clean and rebuild
rebuild: clean all

# Create output directories
dirs:
	@mkdir -p graphviz images

# Run the program (example)
run: $(TARGET) dirs
	./$(TARGET) "(a|b)*c+"

# Show variables (for debugging)
debug:
	@echo "Sources: $(SRCS)"
	@echo "Objects: $(OBJS)"
	@echo "Target: $(TARGET)"

.PHONY: all clean rebuild dirs run debug