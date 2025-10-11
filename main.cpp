#include "include/automata/dfa_minimizer.hpp"
#include "include/automata/nfa_determinizer.hpp"
#include "include/regex/ast_to_nfa.hpp"
#include "include/regex/parser.hpp"
#include "include/regex/scanner.hpp"
#include "include/utils/automata_visualizer.hpp"
#include "include/utils/regex_ast_visualizer.hpp"
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>

using namespace std;

// Helper function to create directory if it doesn't exist
void createDirectory(const String &path) {
#ifdef _WIN32
  _mkdir(path.c_str());
#else
  mkdir(path.c_str(), 0755);
#endif
}

int main(int argc, char *argv[]) {
  // Check if regex was provided as argument
  if (argc < 2) {
    cerr << "Error: No regex pattern provided" << endl;
    cerr << "Usage: " << argv[0] << " <regex_pattern>" << endl;
    cerr << "Example: " << argv[0] << " \"(a|b)*c+\"" << endl;
    return 1;
  }

  String regex = argv[1];
  cout << "Converting regex: " << regex << endl;

  // Create output directories
  createDirectory("graphviz");
  createDirectory("images");

  try {
    // Parse regex to AST
    cout << "\nParsing regex..." << endl;
    RegexScanner scanner(regex);
    RegexParser parser(scanner);
    Pointer<RegexASTNode> ast = parser.parse();
    cout << "AST created successfully" << endl;

    // Visualize AST
    cout << "\nVisualizing AST..." << endl;
    RegexASTVisualizer::visualizeAST(ast.get(), "graphviz/regex_ast",
                                     "images/regex_ast");

    // Convert AST to NFA
    cout << "\nConverting AST to NFA..." << endl;
    NFA nfa = RegexASTToNFA::convert(ast.get());
    cout << "NFA created with " << nfa.getStates().size() << " states" << endl;
    AutomataVisualizer::visualizeNFA(nfa, "graphviz/nfa", "images/nfa");

    // Convert NFA to DFA
    cout << "\nDeterminizing NFA to DFA..." << endl;
    DFA dfa = NFADeterminizer::determinize(nfa);
    cout << "DFA created with " << dfa.getStates().size() << " states" << endl;
    AutomataVisualizer::visualizeDFA(dfa, "graphviz/dfa", "images/dfa");

    // Minimize DFA
    cout << "\nMinimizing DFA..." << endl;
    dfa = DFAMinimizer::minimize(dfa);
    cout << "Minimized DFA created with " << dfa.getStates().size() << " states"
         << endl;
    AutomataVisualizer::visualizeDFA(dfa, "graphviz/minimized_dfa",
                                     "images/minimized_dfa");

    cout << "\nAll visualizations generated successfully!" << endl;
    cout << "DOT files saved in: graphviz/" << endl;
    cout << "Images saved in: images/" << endl;
    cout << "  - images/regex_ast.png (Abstract Syntax Tree)" << endl;
    cout << "  - images/nfa.png (Non-deterministic Finite Automaton)" << endl;
    cout << "  - images/dfa.png (Deterministic Finite Automaton)" << endl;
    cout << "  - images/minimized_dfa.png (Minimized DFA)" << endl;

  } catch (const exception &e) {
    cerr << "Error: " << e.what() << endl;
    return 1;
  }

  return 0;
}