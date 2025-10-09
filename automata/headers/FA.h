#include "State.h"
#include <vector>

class FA {
protected:
  std::vector<State> states;
  std::vector<StateID> accepting_state_ids;
  StateID start_state_id;

public:
  FA() : start_state_id{-1} {}
  FA(const std::vector<State> &states,
     const std::vector<StateID> &accepting_state_ids, StateID start_state_id)
      : states{states}, accepting_state_ids{accepting_state_ids},
        start_state_id{start_state_id} {}
  const std::vector<State> &getStates() const { return states; };
  const std::vector<StateID> &getAcceptingStateIDs() const {
    return accepting_state_ids;
  };
  StateID getStartStateID() const { return start_state_id; };
  const State &getState(StateID id) const { return states.at(id); }
  bool isAccepting(StateID id) const {
    for (StateID state_id : accepting_state_ids) {
      if (state_id == id)
        return true;
    }
    return false;
  }
  virtual void addTransition(StateID from, char symbol, StateID to);
};