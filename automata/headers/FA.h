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
  const std::vector<State> &getStates();
  const std::vector<StateID> &getAcceptingStateIDs();
  StateID getStartStateID();
  void addTransition(StateID from, char symbol, StateID to);
};