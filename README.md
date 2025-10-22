# Scanner Generator

A C++ implementation exploring the theoretical foundations of lexical analysis through finite automata construction.

## Project Status

**Theoretical Implementation Complete, Lexer Generation In Progress**

Currently implements the full automata construction pipeline but does not yet generate actual lexer code. Right now it builds and merges the automata structures in memory—the next phase is outputting executable scanner code.

## What This Actually Does

This is a deep dive into formal language theory and compiler construction. The project implements the classic regex-to-automata pipeline:

1. **Lexical Analysis of Regex**: Custom scanner tokenizes regex patterns (handles escaping, operators, character classes)
2. **Regex Parsing**: Recursive descent parser builds Abstract Syntax Trees from token streams
3. **Thompson's Construction**: Converts AST nodes into NFAs using the elegant compositional approach—each operator has its own NFA fragment that connects via epsilon transitions
4. **Subset Construction**: The classic powerset construction algorithm that determinizes NFAs by tracking sets of states
5. **Hopcroft's Algorithm**: Minimizes DFAs through iterative partition refinement based on transition equivalence classes

The theoretical heavy lifting is done. What's missing is the code generator that outputs actual C/C++ scanner functions.

## Features Implemented

### Regex Support
- Core operators: `|` (alternation), concatenation, `*` (Kleene star), `+` (one or more), `?` (optional)
- Quantifiers: `{n,m}` ranges with unbounded support (`{n,}`)
- Character classes: `[a-z]`, `[0-9]`, `[^abc]` (negated sets)
- Escape sequences: `\n`, `\t`, `\\`, and metacharacter escaping
- Wildcard: `.` (matches printable ASCII 32-126)

### Automata Construction
- Thompson's construction with proper epsilon-NFA handling
- Multi-NFA merging with shared start state (for multiple token types)
- Epsilon closure computation
- Determinization preserving token type information
- State minimization with reachability analysis

### Token Specification Format
Simple DSL for defining tokens:
```
TOKEN_NAME ::= "regex_pattern"
```

Multiple tokens are merged into a single automaton with a common start state.

## Current Output

The program reads `tokens.spec`, builds the complete merged NFA, and reports:
- Total state count
- Accepting states mapped to token types

No lexer code is generated yet—that's the next major milestone.

## Build

```bash
make           # Build
make clean     # Clean artifacts
make rebuild   # Clean and build
```

Requires C++20 for concepts, ranges, and cleaner template syntax.

## Example

Given `tokens.spec`:
```
IDENTIFIER ::= "[a-zA-Z_][a-zA-Z0-9_]*"
INTEGER ::= "0|[1-9][0-9]*"
```

Running `./scanner_generator.exe` produces:
```
Processing token type: IDENTIFIER with regex: [a-zA-Z_][a-zA-Z0-9_]*
Processing token type: INTEGER with regex: 0|[1-9][0-9]*
Successfully created merged NFA with 47 states
Accepting states:
  State 23 -> IDENTIFIER
  State 46 -> INTEGER
```

## Visualization Support

The codebase includes Graphviz integration (see `visualization/` modules) for rendering automata as graphs. Currently unused in main but available for debugging NFA/DFA structures.

## Architecture Highlights

- **Clean separation**: Regex parsing is independent of automata construction
- **Visitor pattern**: AST-to-NFA conversion uses visitor-style dispatch
- **Immutable operations**: Thompson's construction creates new automata rather than mutating
- **Type safety**: Strong typing throughout (StateID, Symbol, Superstate types)

## What's Missing

1. **Code generation**: Output C/C++ scanner functions with transition tables
2. **Token priority**: Longest match and rule precedence
3. **Error handling**: Better diagnostics for invalid regex/specs
4. **Optimization**: Transition table compression, default transitions
5. **Unicode**: Currently ASCII-only

## Theory References

This implementation closely follows the algorithms described in:
- Aho, Sethi, Ullman - "Compilers: Principles, Techniques, and Tools" (The Dragon Book)
- Hopcroft, Ullman - "Introduction to Automata Theory, Languages, and Computation"

The subset construction and minimization are textbook implementations—if you want to understand how these algorithms actually work in practice, this codebase is a pretty direct translation of the theory.
