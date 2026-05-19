cmake_minimum_required(VERSION 3.10)
project(lexy VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Graphviz (optional, but encouraged for visualization)
find_program(DOT_EXECUTABLE dot)
if(DOT_EXECUTABLE)
    message(STATUS "Graphviz 'dot' found: ${DOT_EXECUTABLE}")
    add_definitions(-DHAVE_GRAPHVIZ)
else()
    message(WARNING "Graphviz 'dot' not found. Visualization features will be disabled.")
endif()

# Source files
set(SOURCE_FILES
    main.cpp
    src/automata/nfa.cpp
    src/automata/dfa.cpp
    src/automata/nfa_determinizer.cpp
    src/automata/dfa_minimizer.cpp
    src/automata/thompson_construction.cpp
    src/regex/regex_scanner.cpp
    src/regex/regex_parser.cpp
    src/regex/regex_preprocessor.cpp
    src/regex/regex_ast_to_nfa.cpp
    src/user_specifications/user_spec_scanner.cpp
    src/user_specifications/user_spec_parser.cpp
    src/code_generation/code_generator.cpp
    src/visualization/automata_visualizer.cpp
    src/visualization/regex_ast_visualizer.cpp
)

# Executable
add_executable(lexy ${SOURCE_FILES})

# Include directory
target_include_directories(lexy PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Compiler flags for quality
target_compile_options(lexy PRIVATE -Wall -Wextra -Werror)

# Installation
install(TARGETS lexy RUNTIME DESTINATION bin)
