#include "headers/DFA.h"

void DFA::addTransition(StateID from, Symbol symbol, StateID to) {
  transitions[from][symbol] = to;
}

void DFA::resizeTransitions(size_t new_size) { transitions.resize(new_size); }

StateID DFA::getNextState(StateID from, Symbol symbol) const {
  return transitions[from].at(symbol);
}