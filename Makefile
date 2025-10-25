CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -Isrc

# Source files
SRCS = main.cpp \
       src/automata/nfa.cpp \
       src/automata/dfa.cpp \
       src/automata/nfa_determinizer.cpp \
       src/automata/dfa_minimizer.cpp \
       src/automata/thompson_construction.cpp \
       src/regex/regex_scanner.cpp \
       src/regex/regex_parser.cpp \
       src/regex/regex_preprocessor.cpp \
       src/regex/regex_ast_to_nfa.cpp \
       src/user_specifications/user_spec_scanner.cpp \
       src/user_specifications/user_spec_parser.cpp \
       src/code_generation/code_generator.cpp \
       src/visualization/automata_visualizer.cpp \
       src/visualization/regex_ast_visualizer.cpp

# Object files (in build directory)
BUILD_DIR = build
OBJS = $(SRCS:%.cpp=$(BUILD_DIR)/%.o)

# Executables
TARGET = scanner_generator.exe
TEST_TARGET = test_myScanner.exe

# Default target
all: $(TARGET)

# Link main executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^
	@echo "✓ Build complete: $(TARGET)"

# Compile source files
$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Build test scanner
$(TEST_TARGET): examples/test_myScanner.cpp generated/scanners/myScanner.cpp
	$(CXX) $(CXXFLAGS) -o $@ examples/test_myScanner.cpp
	@echo "✓ Test built: $(TEST_TARGET)"

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR) $(TARGET) $(TEST_TARGET)
	@echo "✓ Cleaned build artifacts"

# Clean everything including generated files
cleanall: clean
	rm -rf generated/*
	@echo "✓ Cleaned all generated files"

# Clean and rebuild
rebuild: clean all

# Create output directories
dirs:
	@mkdir -p generated/scanners generated/graphviz generated/images

# Generate scanner from example
generate: $(TARGET) dirs
	./$(TARGET) examples/myScanner.lexy
	@echo "✓ Scanner generated"

# Build and run test
test: generate $(TEST_TARGET)
	@echo ""
	@echo "Running tests..."
	./$(TEST_TARGET)

# Show variables (for debugging)
debug:
	@echo "Sources: $(SRCS)"
	@echo "Objects: $(OBJS)"
	@echo "Target: $(TARGET)"

.PHONY: all clean cleanall rebuild dirs generate test debug