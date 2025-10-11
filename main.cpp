#include "automata/headers/DFAMinimizer.h"
#include "automata/headers/NFADeterminizer.h"
#include "automata/headers/RegexToNFA.h"
#include "automata/headers/Visualizer.h"
#include <iostream>

using namespace std;

int main() {
  String regex = "(a|b)*|c*|d";
  cout << "Converting regex: " << regex << endl;

  // Convert regex to NFA
  NFA nfa = RegexToNFA::convert(regex);
  cout << "\nNFA created with " << nfa.getStates().size() << " states" << endl;
  Visualizer::visualizeNFA(nfa, "nfa");

  // Convert NFA to DFA
  DFA dfa = NFADeterminizer::determinize(nfa);
  cout << "\nDFA created with " << dfa.getStates().size() << " states" << endl;
  Visualizer::visualizeDFA(dfa, "dfa");

  // Minimize DFA
  dfa = DFAMinimizer::minimize(dfa);
  cout << "\nMinimized DFA created with " << dfa.getStates().size() << " states"
       << endl;
  Visualizer::visualizeDFA(dfa, "minimized_dfa");

  cout << "\nAll visualizations generated successfully!" << endl;

  return 0;
}