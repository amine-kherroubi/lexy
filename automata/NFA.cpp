#include "headers/NFA.h"
#include <utility>

void NFA::addTransition(StateID from, Symbol symbol, StateID to) {
  transitions[from][symbol].push_back(to);
}

void NFA::addEpsilonTransition(StateID from, StateID to) {
  epsilon_transitions[from].push_back(to);
}

const StateIDs &NFA::getNextStates(StateID from, Symbol symbol) const {
  auto iterator = transitions[from].find(symbol);
  if (iterator == transitions[from].end()) {
    static const StateIDs empty;
    return empty;
  }
  return iterator->second;
}

const StateIDs &NFA::getEpsilonNextStates(StateID from) const {
  return epsilon_transitions[from];
}

Symbols NFA::getSymbols(StateID from) const {
  Symbols symbols;
  for (const std::pair<Symbol, StateIDs> &pair : transitions[from]) {
    symbols.push_back(pair.first);
  }
  return symbols;
}