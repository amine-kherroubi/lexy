#include "automata/headers/DFAMinimizer.h"
#include "automata/headers/NFADeterminizer.h"
#include "regex/headers/RegexASTToNFA.h"
#include "regex/headers/RegexParser.h"
#include "regex/headers/RegexScanner.h"
#include "utils/headers/AutomataVisualizer.h"
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

  } catch (const exception &e) {
    cerr << "Error: " << e.what() << endl;
    return 1;
  }

  return 0;
}