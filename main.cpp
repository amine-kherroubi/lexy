#include "automata/headers/NFA.h"
#include "automata/headers/RegexToNFA.h"
#include <iostream>

int main() {
  std::string regex{"((a|b)*c|(ab*c))*"};
  std::cout << "Building NFA for " << regex << std::endl;
  NFA nfa5 = RegexToNFA::convert(regex);
  std::cout << "Number of states: " << nfa5.getStates().size() << std::endl;
  return 0;
}