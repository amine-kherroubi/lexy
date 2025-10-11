#include "../../include/automata/dfa.hpp"

void DFA::addTransition(StateID from, Symbol symbol, StateID to) {
  transitions[from][symbol] = to;
}

void DFA::resizeTransitions(Size new_size) { transitions.resize(new_size); }

StateID DFA::getNextState(StateID from, Symbol symbol) const {
  if (from < 0 || from >= static_cast<int>(transitions.size())) {
    return -1;
  }

  auto iterator = transitions[from].find(symbol);
  if (iterator == transitions[from].end()) {
    return -1;
  }
  return iterator->second;
}