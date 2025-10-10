#pragma once
#include "Alphabet.h"
#include "State.h"
#include <vector>
using States = std::vector<State>;
using StateIDs = std::vector<StateID>;

class FA {
protected:
  States states;
  StateIDs accepting_state_ids;
  StateID start_state_id;

public:
  FA() : start_state_id{-1} {}
  FA(const States &states, const StateIDs &accepting_state_ids,
     StateID start_state_id)
      : states{states}, accepting_state_ids{accepting_state_ids},
        start_state_id{start_state_id} {}

  States &getStates() { return states; }
  const States &getStates() const { return states; }

  StateIDs &getAcceptingStateIDs() { return accepting_state_ids; }
  const StateIDs &getAcceptingStateIDs() const { return accepting_state_ids; }

  StateID getStartStateID() const { return start_state_id; }
  void setStartStateID(StateID id) { this->start_state_id = id; }

  State &getState(StateID id) { return states.at(id); }
  const State &getState(StateID id) const { return states.at(id); }

  bool isAccepting(StateID id) const {
    for (StateID state_id : accepting_state_ids) {
      if (state_id == id)
        return true;
    }
    return false;
  }
};