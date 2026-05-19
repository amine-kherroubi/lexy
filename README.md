# Lexy

![C++](https://img.shields.io/badge/C%2B%2B-20-00599C?style=flat&logo=c%2B%2B&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-%3E%3D3.10-064F8C?style=flat&logo=cmake)
![Platform](https://img.shields.io/badge/Platform-Linux-FCC624?style=flat&logo=linux&logoColor=black)
![License](https://img.shields.io/badge/License-MIT-yellow.svg)

Lexy is a professional-grade lexical analyzer generator that compiles regular expression specifications into efficient, table-driven C++ scanners.

## Features
- **Standardized Build System**: Uses CMake for cross-platform portability.
- **Professional CLI**: Flexible input/output configuration via command-line arguments.
- **Automata Visualization**: Optional generation of visual representations (Graphviz) of NFA and DFA construction stages.
- **Header-Driven**: Designed for integration into larger compiler projects.

## Supported Platforms
Lexy is only supported on **Linux**.

## Prerequisites
- C++20 compatible compiler (e.g., `g++` or `clang++`)
- `cmake` (>= 3.10)
- Graphviz (`dot` command, optional for visualization)

## Installation
### From Source
```bash
cmake -B build
cmake --build build
sudo cmake --install build  # Installs the 'lexy' binary to /usr/local/bin
```
Alternatively, you can run the `lexy` executable directly from the `build` directory.

## Usage
```bash
# Using installed binary
lexy <input_spec.lexy> [options]

# Using local build
./build/lexy <input_spec.lexy> [options]
```

**Options:**
- `<input_spec.lexy>`: Input specification file (required). Note: Special regex characters (e.g., `+`, `{`, `}`, `(`, `)`, `!`, `->`) must be escaped with a backslash (e.g., `"\+"`) in the specification file.
- `-o <dir>`: Output directory for scanner code and visualizations (default: `./output`).
- `-g`: Enable automata graph generation (disabled by default).
- `-h`: Show help message.

## Example
You can test `lexy` using the provided sample files in the `example` folder.

**Input Specification** (`example/sample_scanner.lexy`):
```lexy
FN         ::= "fn"
LET        ::= "let"
MUT        ::= "mut"
PRINT      ::= "println\!"
I32        ::= "i32"
RETURN     ::= "return"
ARROW      ::= "\->"
IDENTIFIER ::= "[a-zA-Z_][a-zA-Z0-9_]*"
INTEGER    ::= "[0-9]+"
ASSIGN     ::= "="
SEMICOLON  ::= ";"
LBRACE     ::= "\{"
RBRACE     ::= "\}"
LPAREN     ::= "\("
RPAREN     ::= "\)"
COLON      ::= ":"
PLUS       ::= "\+"
WHITESPACE ::= "[ \t\n]+"
```

**Input Program** (`example/sample_program.rs`):
```rust
fn add(mut x: i32) -> i32 {
let y = 10;
println!("Result");
return x + y;
}
```

## Example Usage (Integrating the Scanner)
After generating the scanner (`output/scanners/sample_scanner.cpp`), you can integrate it into your own C++ project.

Here is a simple example program (`test.cpp`) to test the scanner:
```cpp
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "output/scanners/sample_scanner.cpp"

int main() {
    // Read the input program file
std::ifstream file("example/sample_program.rs");
if (!file.is_open()) {
std::cerr << "Failed to open input file." << std::endl;
return 1;
    }
std::stringstream buffer;
    buffer << file.rdbuf();
std::string content = buffer.str();

    // Initialize scanner
    Scanner scanner(content.c_str());

    // Scan and print tokens
    Token token;
while ((token = scanner.getNextToken()).type != -1) {
if (token.type == -2) {
std::cout << "Unknown token: " << token.lexeme << std::endl;
        } else {
std::cout << "Token: " << TOKEN_NAMES[token.type]
<< " | Lexeme: '" << token.lexeme << "'" << std::endl;
        }
    }

return 0;
}
```

### Compiling and Running
To compile your test program along with the generated scanner:
```bash
# Compile the test program
g++ -std=c++20 test.cpp -o scanner_tester

# Run the tester
./scanner_tester
```

## References
- Aho, Sethi, Ullman - *Compilers: Principles, Techniques, and Tools* (Dragon Book)
- Cooper & Torczon - *Engineering a Compiler*
- Hopcroft, Motwani, Ullman - *Introduction to Automata Theory*
