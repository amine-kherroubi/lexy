#include "headers/DFA.h"

void DFA::addTransition(StateID from, Symbol symbol, StateID to) {
  transitions[from][symbol] = to;
}

void DFA::resizeTransitions(size_t new_size) { transitions.resize(new_size); }

StateID DFA::getNextState(StateID from, Symbol symbol) const {
  if (from < 0 || from >= static_cast<int>(transitions.size())) {
    return -1;
  }

  auto it = transitions[from].find(symbol);
  if (it == transitions[from].end()) {
    return -1;
  }
  return it->second;
}