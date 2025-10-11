#include "automata/headers/DFAMinimizer.h"
#include "automata/headers/NFADeterminizer.h"
#include "automata/headers/RegexToNFA.h"
#include "automata/headers/Visualizer.h"
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
    cerr << "Example: " << argv[0] << " \"(a|b)*|c*|d\"" << endl;
    return 1;
  }

  String regex = argv[1];
  cout << "Converting regex: " << regex << endl;

  // Create output directories
  createDirectory("graphviz");
  createDirectory("images");

  try {
    // Convert regex to NFA
    NFA nfa = RegexToNFA::convert(regex);
    cout << "\nNFA created with " << nfa.getStates().size() << " states"
         << endl;
    Visualizer::visualizeNFA(nfa, "graphviz/nfa", "images/nfa");

    // Convert NFA to DFA
    DFA dfa = NFADeterminizer::determinize(nfa);
    cout << "\nDFA created with " << dfa.getStates().size() << " states"
         << endl;
    Visualizer::visualizeDFA(dfa, "graphviz/dfa", "images/dfa");

    // Minimize DFA
    dfa = DFAMinimizer::minimize(dfa);
    cout << "\nMinimized DFA created with " << dfa.getStates().size()
         << " states" << endl;
    Visualizer::visualizeDFA(dfa, "graphviz/minimized_dfa",
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