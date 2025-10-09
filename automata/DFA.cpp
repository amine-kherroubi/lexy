#include "headers/DFA.h"

void DFA::addTransition(StateID from, Symbol symbol, StateID to) {
  transitions[from][symbol] = to;
}

StateID DFA::getNextState(StateID from, Symbol symbol) const {
  return transitions[from].at(symbol);
}