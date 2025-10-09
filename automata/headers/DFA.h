#include "FA.h"
#include <unordered_map>

class DFA : public FA {
private:
  std::vector<std::unordered_map<char, StateID>> transitions;

public:
  DFA() = default;
  DFA(const std::vector<State> &states,
      const std::vector<StateID> &accepting_state_ids, StateID start_state_id)
      : FA{states, accepting_state_ids, start_state_id},
        transitions(states.size()) {}
  StateID getNextState(StateID from, char symbol) const;
};