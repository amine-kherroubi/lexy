#include "automata/headers/DFAMinimizer.h"
#include "automata/headers/NFADeterminizer.h"
#include "automata/headers/RegexToNFA.h"
#include "automata/headers/Visualizer.h"
#include <iostream>

int main() {
  String regex = "a|b";
  std::cout << "Converting regex: " << regex << std::endl;

  // Convert regex to NFA
  NFA nfa = RegexToNFA::convert(regex);
  std::cout << "\nNFA created with " << nfa.getStates().size() << " states"
            << std::endl;
  Visualizer::visualizeNFA(nfa, "nfa");

  // Convert NFA to DFA
  DFA dfa = NFADeterminizer::determinize(nfa);
  std::cout << "\nDFA created with " << dfa.getStates().size() << " states"
            << std::endl;
  Visualizer::visualizeDFA(dfa, "dfa");

  // Minimize DFA
  dfa = DFAMinimizer::minimize(dfa);
  std::cout << "\nMinimized DFA created with " << dfa.getStates().size()
            << " states" << std::endl;
  Visualizer::visualizeDFA(dfa, "minimized_dfa");

  std::cout << "\n✓ All visualizations generated successfully!" << std::endl;

  return 0;
}