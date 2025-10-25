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

```bash
make                                   # Build generator
./scanner_generator.exe input.lexy     # Generate scanner
```

Requires C++20.

## Example

**Input** (`examples/myScanner.lexy`):
```
IDENTIFIER ::= "[a-zA-Z_][a-zA-Z0-9_]*"
INTEGER ::= "0|[1-9][0-9]*"
```

**Generate:**
```bash
./scanner_generator.exe examples/myScanner.lexy
```

**Output:** `generated/scanners/myScanner.cpp`

**Test:**
```cpp
Scanner scanner("hello123");
Token t1 = scanner.getNextToken();  // IDENTIFIER: "hello"
Token t2 = scanner.getNextToken();  // INTEGER: "123"
Token t3 = scanner.getNextToken();  // EOF
```

## Limitations

- ASCII only (0-127)
- No table compression
- No token priority rules
- No whitespace skipping

## References

- Aho, Sethi, Ullman - *Compilers: Principles, Techniques, and Tools* (Dragon Book)
- Cooper & Torczon - *Engineering a Compiler*
- Hopcroft, Motwani, Ullman - *Introduction to Automata Theory*
