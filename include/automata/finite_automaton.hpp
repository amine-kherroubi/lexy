#pragma once

#include "../core/types.hpp"

class FA {
protected:
  Alphabet alphabet_;
  States states_;
  StateIDs accepting_state_ids_;
  StateID start_state_id_;

public:
  FA(const Alphabet &alphabet, const States &states,
     const StateIDs &accepting_state_ids, StateID start_state_id)
      : alphabet_(alphabet), states_(states),
        accepting_state_ids_(accepting_state_ids),
        start_state_id_(start_state_id) {}

  // Const getters (read-only, return by value for simple types)
  Alphabet getAlphabet() const { return alphabet_; }
  States getStates() const { return states_; }
  StateIDs getAcceptingStateIDs() const { return accepting_state_ids_; }
  StateID getStartStateID() const { return start_state_id_; }

  // Non-const getters (allow modification, return by reference)
  Alphabet &getAlphabet() { return alphabet_; }
  States &getStates() { return states_; }
  StateIDs &getAcceptingStateIDs() { return accepting_state_ids_; }

  // Setters
  void setStartStateID(StateID id) { this->start_state_id_ = id; }

  // Others
  bool isAccepting(StateID id) const {
    for (StateID state_id : accepting_state_ids_) {
      if (state_id == id)
        return true;
    }
    return false;
  }
};