#include "State.h"
#include <vector>

class FA {
protected:
  std::vector<State> states;
  StateID start_state_id;

public:
  FA() : start_state_id{-1} {}
  FA(const std::vector<State> &states, StateID start_state_id)
      : states{states}, start_state_id{start_state_id} {}
};