#include "headers/NFA.h"
#include <vector>

void NFA::addTransition(StateID from, char symbol, StateID to) {
  transitions[from][symbol].push_back(to);
}

void NFA::addEpsilonTransition(StateID from, StateID to) {
  epsilon_transitions[from].push_back(to);
}

const std::vector<StateID> &NFA::getNextStates(StateID from,
                                               char symbol) const {
  auto it = transitions[from].find(symbol);
  if (it == transitions[from].end()) {
    static const std::vector<StateID> empty;
    return empty;
  }
  return it->second;
}

const std::vector<StateID> &NFA::getEpsilonNextStates(StateID from) const {
  return epsilon_transitions[from];
}