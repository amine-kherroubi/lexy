#include "FA.h"
#include <unordered_map>
#include <vector>

class NFA : public FA {
private:
  std::vector<std::unordered_map<char, std::vector<StateID>>> transitions;
  std::vector<std::vector<StateID>> epsilon_transitions;

public:
  NFA() = default;
  NFA(const std::vector<State> &states, StateID start_state_id)
      : FA{states, start_state_id}, transitions(states.size()),
        epsilon_transitions(states.size()) {}

  void addTransition(StateID from, char symbol, StateID to);
  void addEpsilonTransition(StateID from, StateID to);
  const std::vector<StateID> &getNextStates(StateID from, char symbol) const;
  const std::vector<StateID> &getEpsilonNextStates(StateID from) const;
};