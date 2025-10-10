#include "automata/headers/NFA.h"
#include "automata/headers/RegexToNFA.h"
#include <iostream>

int main() {
  // Test 1: Simple symbol
  std::cout << "Test 1: Building NFA for 'a'" << std::endl;
  NFA nfa1 = RegexToNFA::convert("a");
  std::cout << "States: " << nfa1.getStates().size() << std::endl;
  std::cout << "Start state: " << nfa1.getStartStateID() << std::endl;
  std::cout << "Accepting states: ";
  for (StateID id : nfa1.getAcceptingStateIDs()) {
    std::cout << id << " ";
  }
  std::cout << "\n\n";

  // Test 2: Concatenation
  std::cout << "Test 2: Building NFA for 'ab'" << std::endl;
  NFA nfa2 = RegexToNFA::convert("ab");
  std::cout << "States: " << nfa2.getStates().size() << std::endl;
  std::cout << "Start state: " << nfa2.getStartStateID() << std::endl;
  std::cout << "Accepting states: ";
  for (StateID id : nfa2.getAcceptingStateIDs()) {
    std::cout << id << " ";
  }
  std::cout << "\n\n";

  // Test 3: Alternation
  std::cout << "Test 3: Building NFA for 'a|b'" << std::endl;
  NFA nfa3 = RegexToNFA::convert("a|b");
  std::cout << "States: " << nfa3.getStates().size() << std::endl;
  std::cout << "Start state: " << nfa3.getStartStateID() << std::endl;
  std::cout << "Accepting states: ";
  for (StateID id : nfa3.getAcceptingStateIDs()) {
    std::cout << id << " ";
  }
  std::cout << "\n\n";

  // Test 4: Kleene star
  std::cout << "Test 4: Building NFA for 'a*'" << std::endl;
  NFA nfa4 = RegexToNFA::convert("a*");
  std::cout << "States: " << nfa4.getStates().size() << std::endl;
  std::cout << "Start state: " << nfa4.getStartStateID() << std::endl;
  std::cout << "Accepting states: ";
  for (StateID id : nfa4.getAcceptingStateIDs()) {
    std::cout << id << " ";
  }
  std::cout << "\n\n";

  // Test 5: Complex regex
  std::cout << "Test 5: Building NFA for '(a|b)*c'" << std::endl;
  NFA nfa5 = RegexToNFA::convert("(a|b)*c");
  std::cout << "States: " << nfa5.getStates().size() << std::endl;
  std::cout << "Start state: " << nfa5.getStartStateID() << std::endl;
  std::cout << "Accepting states: ";
  for (StateID id : nfa5.getAcceptingStateIDs()) {
    std::cout << id << " ";
  }
  std::cout << "\n\n";

  std::cout << "All tests completed!" << std::endl;

  return 0;
}