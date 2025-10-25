#include "nfa.hpp"

void NFA::addTransition(StateID from, Symbol symbol, StateID to) {
  transitions_[from][symbol].push_back(to);
}

void NFA::addEpsilonTransition(StateID from, StateID to) {
  epsilon_transitions_[from].push_back(to);
}

void NFA::resizeTransitions(Size new_size) {
  transitions_.resize(new_size);
  epsilon_transitions_.resize(new_size);
}

StateIDs NFA::getNextStateIDs(StateID from, Symbol symbol) const {
  if (from < 0 || from >= static_cast<int>(transitions_.size())) {
    return StateIDs();
  }

  auto iterator = transitions_[from].find(symbol);
  if (iterator == transitions_[from].end()) {
    return StateIDs();
  }
  return iterator->second;
}

StateIDs NFA::getEpsilonNextStatesIDs(StateID from) const {
  if (from < 0 || from >= static_cast<int>(epsilon_transitions_.size())) {
    return StateIDs();
  }
  return epsilon_transitions_[from];
}

Symbols NFA::getSymbols(StateID from) const {
  Symbols symbols;
  if (from < 0 || from >= static_cast<int>(transitions_.size())) {
    return symbols;
  }

  for (const auto &[symbol, state_ids] : transitions_[from]) {
    symbols.push_back(symbol);
  }
  return symbols;
}