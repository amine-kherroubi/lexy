#pragma once

#include "../../global/types.h"
#include "FA.h"

class DFA : public FA {
private:
  Vector<UnorderedMap<Symbol, StateID>> transitions;

public:
  DFA(const Alphabet &alphabet, const States &states,
      const StateIDs &accepting_state_ids, StateID start_state_id)
      : FA(alphabet, states, accepting_state_ids, start_state_id),
        transitions(states.size()) {}

  void addTransition(StateID, Symbol, StateID);
  void resizeTransitions(Size);

  StateID getNextState(StateID, Symbol) const;
};