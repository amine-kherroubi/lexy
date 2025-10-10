#include "headers/NFA.h"

void NFA::addTransition(StateID from, Symbol symbol, StateID to) {
  transitions[from][symbol].push_back(to);
}

void NFA::addEpsilonTransition(StateID from, StateID to) {
  epsilon_transitions[from].push_back(to);
}

void NFA::resizeTransitions(size_t new_size) {
  transitions.resize(new_size);
  epsilon_transitions.resize(new_size);
}

StateIDs NFA::getNextStateIDs(StateID from, Symbol symbol) const {
  if (from < 0 || from >= static_cast<int>(transitions.size())) {
    return StateIDs();
  }

  auto it = transitions[from].find(symbol);
  if (it == transitions[from].end()) {
    return StateIDs();
  }
  return it->second;
}

StateIDs NFA::getEpsilonNextStatesIDs(StateID from) const {
  if (from < 0 || from >= static_cast<int>(epsilon_transitions.size())) {
    return StateIDs();
  }
  return epsilon_transitions[from];
}

Symbols NFA::getSymbols(StateID from) const {
  Symbols symbols;
  if (from < 0 || from >= static_cast<int>(transitions.size())) {
    return symbols;
  }

  for (const auto &[symbol, state_ids] : transitions[from]) {
    symbols.push_back(symbol);
  }
  return symbols;
}