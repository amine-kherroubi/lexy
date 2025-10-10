#include "automata/headers/NFADeterminizer.h"
#include "automata/headers/RegexToNFA.h"

int main() {
  String regex = "((a|b)*c|(ab*c))*";
  NFA nfa = RegexToNFA::convert(regex);
  DFA dfa = NFADeterminizer::determinize(nfa);
  return 0;
}