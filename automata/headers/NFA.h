#pragma once
#include "FA.h"
#include <unordered_map>
#include <vector>
using NondeterministicTransitions =
    std::vector<std::unordered_map<Symbol, StateIDs>>;
using EpsilonTransitions = std::vector<StateIDs>;

class NFA : public FA {
private:
  NondeterministicTransitions transitions;
  EpsilonTransitions epsilon_transitions;

public:
  NFA() = default;
  NFA(const States &states, const StateIDs &accepting_state_ids,
      StateID start_state_id)
      : FA{states, accepting_state_ids, start_state_id},
        transitions(states.size()), epsilon_transitions(states.size()) {}
  void addEpsilonTransition(StateID from, StateID to);
  const StateIDs &getNextStates(StateID from, Symbol symbol) const;
  const StateIDs &getEpsilonNextStates(StateID from) const;
  void addTransition(StateID from, Symbol symbol, StateID to) override;
  Symbols getSymbols(StateID from) const;
};