#pragma once

#include "../common/types.hpp"
#include "fa.hpp"

class DFA : public FA {
private:
  Vector<UnorderedMap<Symbol, StateID>> transitions_;

public:
  DFA(const Alphabet &alphabet, const States &states,
      const UnorderedMap<StateID, String> &accepting_states,
      StateID start_state_id)
      : FA(alphabet, states, accepting_states, start_state_id),
        transitions_(states.size()) {}

  void addTransition(StateID, Symbol, StateID);
  void resizeTransitions(Size);

  StateID getNextState(StateID, Symbol) const;
};