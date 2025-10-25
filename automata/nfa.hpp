#pragma once

#include "../common/types.hpp"
#include "fa.hpp"

class NFA : public FA {
private:
  Vector<UnorderedMap<Symbol, StateIDs>> transitions_;
  Vector<StateIDs> epsilon_transitions_;

public:
  NFA(const Alphabet &alphabet, const States &states,
      const UnorderedMap<StateID, String> &accepting_states,
      StateID start_state_id)
      : FA(alphabet, states, accepting_states, start_state_id),
        transitions_(states.size()), epsilon_transitions_(states.size()) {}

  void addTransition(StateID, Symbol, StateID);
  void addEpsilonTransition(StateID, StateID);
  void resizeTransitions(Size);

  StateIDs getNextStateIDs(StateID, Symbol) const;
  StateIDs getEpsilonNextStatesIDs(StateID) const;
  Symbols getSymbols(StateID) const;
};