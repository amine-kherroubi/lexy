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

  void addTransition(StateID from, Symbol symbol, StateID to);
  void addEpsilonTransition(StateID from, StateID to);
  void resizeTransitions(Size new_size);

  StateIDs getNextStateIDs(StateID from, Symbol symbol) const;
  StateIDs getEpsilonNextStatesIDs(StateID from) const;
  Symbols getSymbols(StateID from) const;
};