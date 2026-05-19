# Lexy

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
mkdir build && cd build
cmake ..
make
sudo make install  # Installs the 'lexy' binary to /usr/local/bin
```

Alternatively, you can run the `lexy` executable directly from the `build` directory.

## Usage

```bash
lexy <input_spec.lexy> [options]
```

**Options:**
- `<input_spec.lexy>`: Input specification file (required).
- `-o <dir>`: Output directory for scanner code and visualizations (default: `./output`).
- `-g`: Enable automata graph generation (disabled by default).
- `-h`: Show help message.

## Example

**Input** (`scanner.lexy`):
```lexy
IDENTIFIER ::= "[a-zA-Z_][a-zA-Z0-9_]*"
INTEGER    ::= "0|[1-9][0-9]*"
```

**Run (with graph generation):**
```bash
lexy scanner.lexy -o ./build_scanner -g
```

This will generate:
- `build_scanner/scanners/scanner.cpp` (The scanner code)
- `build_scanner/images/` (Visualizations: `nfa.png`, `dfa.png`, `dfa_minimized.png`)

## Testing

The project includes a test suite to verify both the CLI generator and the generated scanners.

### Running Tests

```bash
./tests/test.sh
```

This script will:
1. Build the `lexy` executable.
2. Run the generator on `tests/myScanner.lexy`.
3. Verify the generation of scanner code and visualization files.
4. Compile and run the generated scanner test (`tests/test_myScanner.cpp`), which reads `tests/sample_program.txt`.

## Example

**Input Specification** (`tests/myScanner.lexy`):
```lexy
FN         ::= "fn"
LET        ::= "let"
MUT        ::= "mut"
PRINT      ::= "println!"
I32        ::= "i32"
RETURN     ::= "return"
ARROW      ::= "->"
IDENTIFIER ::= "[a-zA-Z_][a-zA-Z0-9_]*"
INTEGER    ::= "[0-9]+"
ASSIGN     ::= "="
SEMICOLON  ::= ";"
LBRACE     ::= "{"
RBRACE     ::= "}"
LPAREN     ::= "("
RPAREN     ::= ")"
COLON      ::= ":"
PLUS       ::= "+"
WHITESPACE ::= "[ \t\n]+"
```

**Input Program** (`tests/sample_program.rs`):
```rust
fn add(mut x: i32) -> i32 {
    let y = 10;
    println!("Result");
    return x + y;
}
```

This script will:
1. Build the `lexy` executable.
2. Run the generator on `examples/myScanner.lexy`.
3. Verify the generation of scanner code and visualization files.
4. Compile and run the generated scanner test (`tests/test_myScanner.cpp`).

## References

- Aho, Sethi, Ullman - *Compilers: Principles, Techniques, and Tools* (Dragon Book)
- Cooper & Torczon - *Engineering a Compiler*
- Hopcroft, Motwani, Ullman - *Introduction to Automata Theory*
