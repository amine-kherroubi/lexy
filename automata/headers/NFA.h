#pragma once

#include "../../global/types.h"
#include "FA.h"

class NFA : public FA {
private:
  Vector<UnorderedMap<Symbol, StateIDs>> transitions;
  Vector<StateIDs> epsilon_transitions;

public:
  NFA(const Alphabet &alphabet, const States &states,
      const StateIDs &accepting_state_ids, StateID start_state_id)
      : FA(alphabet, states, accepting_state_ids, start_state_id),
        transitions(states.size()), epsilon_transitions(states.size()) {}

  void addTransition(StateID, Symbol, StateID);
  void addEpsilonTransition(StateID, StateID);
  void resizeTransitions(Size);

  StateIDs getNextStateIDs(StateID, Symbol) const;
  StateIDs getEpsilonNextStatesIDs(StateID) const;
  Symbols getSymbols(StateID) const;
};