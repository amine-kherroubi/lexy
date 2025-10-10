#pragma once

#include "../../global/types.h"

class FA {
protected:
  Alphabet alphabet;
  States states;
  StateIDs accepting_state_ids;
  StateID start_state_id;

public:
  FA(const Alphabet &alphabet, const States &states,
     const StateIDs &accepting_state_ids, StateID start_state_id)
      : alphabet(alphabet), states(states),
        accepting_state_ids(accepting_state_ids),
        start_state_id(start_state_id) {}

  Alphabet &getAlphabet() { return alphabet; }
  States &getStates() { return states; }
  StateIDs &getAcceptingStateIDs() { return accepting_state_ids; }

  StateID getStartStateID() const { return start_state_id; }
  void setStartStateID(StateID id) { this->start_state_id = id; }

  bool isAccepting(StateID id) const {
    for (StateID state_id : accepting_state_ids) {
      if (state_id == id)
        return true;
    }
    return false;
  }
};