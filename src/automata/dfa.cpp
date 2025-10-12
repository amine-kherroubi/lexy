#include "../../include/automata/dfa.hpp"

void DFA::addTransition(StateID from, Symbol symbol, StateID to) {
  transitions_[from][symbol] = to;
}

void DFA::resizeTransitions(Size new_size) { transitions_.resize(new_size); }

StateID DFA::getNextState(StateID from, Symbol symbol) const {
  if (from < 0 || from >= static_cast<int>(transitions_.size())) {
    return -1;
  }

  auto iterator = transitions_[from].find(symbol);
  if (iterator == transitions_[from].end()) {
    return -1;
  }
  return iterator->second;
}