#include "FA.h"
#include <unordered_map>

class DFA : public FA {
private:
  std::vector<std::unordered_map<char, StateID>> transitions;

public:
  DFA() = default;
  DFA(const std::vector<State> &states, StateID start_state_id)
      : FA{states, start_state_id}, transitions(states.size()) {}

  void addTransition(StateID from, char symbol, StateID to);
  StateID getNextState(StateID from, char symbol) const;
};