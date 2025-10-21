#pragma once

#include "../core/helpers.hpp"
#include "../core/types.hpp"

class FA {
protected:
  Alphabet alphabet_;
  States states_;
  UnorderedMap<StateID, String> accepting_state_ids_to_token_types_;
  StateID start_state_id_;

public:
  FA(const Alphabet &alphabet, const States &states,
     const UnorderedMap<StateID, String> &accepting_states,
     StateID start_state_id)
      : alphabet_(alphabet), states_(states),
        accepting_state_ids_to_token_types_(accepting_states),
        start_state_id_(start_state_id) {}

  // Const getters
  Alphabet getAlphabet() const { return alphabet_; }
  States getStates() const { return states_; }
  StateIDs getAcceptingStateIDs() const {
    return get_keys(accepting_state_ids_to_token_types_);
  }
  StateID getStartStateID() const { return start_state_id_; }

  // Non-const getters
  Alphabet &getAlphabet() { return alphabet_; }
  States &getStates() { return states_; }
  UnorderedMap<StateID, String> &getAcceptingStateIDsToTokenTypes() {
    return accepting_state_ids_to_token_types_;
  }

  // Setters
  void setStartStateID(StateID id) { start_state_id_ = id; }

  // Others
  bool isAccepting(StateID id) const {
    return accepting_state_ids_to_token_types_.find(id) !=
           accepting_state_ids_to_token_types_.end();
  }

  String getTokenType(StateID id) const {
    auto iterator = accepting_state_ids_to_token_types_.find(id);
    return iterator != accepting_state_ids_to_token_types_.end()
               ? iterator->second
               : String{};
  }
};