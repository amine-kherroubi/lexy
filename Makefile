CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra

# Source files (now co-located with headers)
SRCS = main.cpp \
       automata/nfa.cpp \
       automata/dfa.cpp \
       automata/nfa_determinizer.cpp \
       automata/dfa_minimizer.cpp \
       automata/thompson_construction.cpp \
       regex/scanner.cpp \
       regex/parser.cpp \
       regex/preprocessor.cpp \
       regex/ast_to_nfa.cpp \
       user_specifications/user_spec_scanner.cpp \
       user_specifications/user_spec_parser.cpp \
       code_generation/code_generator.cpp \
       visualization/automata_visualizer.cpp \
       visualization/regex_ast_visualizer.cpp

# Object files (in build directory)
BUILD_DIR = build
OBJS = $(SRCS:%.cpp=$(BUILD_DIR)/%.o)

# Target executable
TARGET = scanner_generator.exe
TEST_TARGET = test_scanner.exe

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

# Build test scanner
$(TEST_TARGET): tests/test_scanner.cpp generated/myScanner.cpp
	$(CXX) $(CXXFLAGS) -o $@ tests/test_scanner.cpp
	@echo "Test scanner built: $(TEST_TARGET)"

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR) $(TARGET) $(TEST_TARGET)
	rm -rf graphviz/ images/

# Clean generated scanners too
cleanall: clean
	rm -rf generated/*.cpp

# Clean and rebuild
rebuild: clean all

# Create output directories
dirs:
	@mkdir -p graphviz images generated

# Generate scanner from example
generate: $(TARGET) dirs
	./$(TARGET) examples/myScanner.lexy

# Build and run test
test: generate $(TEST_TARGET)
	./$(TEST_TARGET)

# Show variables (for debugging)
debug:
	@echo "Sources: $(SRCS)"
	@echo "Objects: $(OBJS)"
	@echo "Target: $(TARGET)"

.PHONY: all clean cleanall rebuild dirs generate test debug