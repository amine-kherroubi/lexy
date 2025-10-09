#include "headers/DFA.h"

void DFA::addTransition(StateID from, char symbol, StateID to) {
  transitions[from][symbol] = to;
}

StateID DFA::getNextState(StateID from, char symbol) const {
  return transitions[from].at(symbol);
}