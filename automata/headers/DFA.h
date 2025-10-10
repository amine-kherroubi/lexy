#pragma once
#include "FA.h"
#include <unordered_map>
using DeterministicTransitions =
    std::vector<std::unordered_map<Symbol, StateID>>;

class DFA : public FA {
private:
  DeterministicTransitions transitions;

public:
  DFA() = default;
  DFA(const States &states, const StateIDs &accepting_state_ids,
      StateID start_state_id)
      : FA{states, accepting_state_ids, start_state_id},
        transitions(states.size()) {}
  StateID getNextState(StateID from, Symbol symbol) const;
  void addTransition(StateID from, Symbol symbol, StateID to);
};