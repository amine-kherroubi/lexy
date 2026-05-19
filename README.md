# Lexy

A lexical analyzer generator. Compiles regex patterns into C++ table-driven scanners.

## What It Does

Lexy takes token specifications and generates standalone C++ scanners. The generated code uses transition tables to recognize tokens—same approach as Lex/Flex.

**Pipeline:**
1. Parse `.lexy` token specifications
2. Build regex ASTs
3. Convert ASTs to NFAs (Thompson's construction)
4. Merge multiple NFAs into one
5. Determinize to DFA (subset construction)
6. Minimize DFA (Hopcroft's algorithm)
7. Generate C++ code with transition tables

**Table-Driven Design:**

The generated scanners use a 2D array `TRANSITION_TABLE[state][char] -> next_state` plus an accepting states array. A simple loop walks the input, looks up transitions, and implements longest-match with backtracking.

## Regex Support
- Operators: `|`, `*`, `+`, `?`, and concatenation
- Ranges: `{n,m}`, `{n,}`
- Character classes: `[a-z]`, `[^abc]`
- Escapes: `\n`, `\t`, `\\`, and metacharacters
- Wildcard: `.`

## Build & Run

### Prerequisites
- C++20 compatible compiler (e.g., `clang++`)
- `make`
- Graphviz (`dot` command for visualization)

### Building the Generator
```bash
make
```

### Generating a Scanner
```bash
./scanner_generator.exe <path_to_spec.lexy>
```
The generated scanner will be located in `generated/scanners/`, and visual representations of the constructed automata (NFA, DFA, and Minimized DFA) will be generated in `generated/images/`.

## Testing

To verify the correct functionality of the scanner generator and the generated code, execute the following command:

```bash
make test
```

This command automatically performs the following steps:
1. Rebuilds the generator if necessary.
2. Generates the scanner based on the example specification (`examples/myScanner.lexy`).
3. Compiles the test program (`examples/test_myScanner.cpp`).
4. Executes the test suite and validates output against expected token sequences.

Successful execution will indicate `12/12 passed` for the provided test cases.

## Limitations

- ASCII only (0-127)
- No table compression
- No token priority rules
- No whitespace skipping

## References

- Aho, Sethi, Ullman - *Compilers: Principles, Techniques, and Tools* (Dragon Book)
- Cooper & Torczon - *Engineering a Compiler*
- Hopcroft, Motwani, Ullman - *Introduction to Automata Theory*
