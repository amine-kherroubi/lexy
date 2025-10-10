#include "automata/headers/DFAMinimizer.h"
#include "automata/headers/NFADeterminizer.h"
#include "automata/headers/RegexToNFA.h"

int main() {
  String regex = "((a|b)*c|(ab*c))*";
  NFA nfa = RegexToNFA::convert(regex);
  DFA dfa = NFADeterminizer::determinize(nfa);
  dfa = DFAMinimizer::minimize(dfa);
  return 0;
}