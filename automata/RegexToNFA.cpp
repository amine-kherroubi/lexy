#include "headers/RegexToNFA.h"
#include <cstddef>

NFA RegexToNFA::buildForSymbol(Symbol c) {
  const StateID start_state_id{0};
  const StateID accept_state_id{1};
  const States states{State{start_state_id}, State{accept_state_id}};
  const StateIDs accepting_state_ids{accept_state_id};

  NFA nfa{states, accepting_state_ids, start_state_id};
  nfa.addTransition(start_state_id, c, accept_state_id);
  return nfa;
}

NFA RegexToNFA::concatenate(const NFA &left, const NFA &right) {
  States combined_states{left.getStates()};
  int left_size = combined_states.size();

  States right_states{right.getStates()};
  for (State &state : right_states) {
    StateID old_id = state.getID();
    StateID new_id = left_size + old_id;
    state.setID(new_id);
  }

  combined_states.insert(combined_states.end(), right_states.begin(),
                         right_states.end());

  StateIDs new_accepting_ids;
  for (StateID old_id : right.getAcceptingStateIDs()) {
    new_accepting_ids.push_back(left_size + old_id);
  }

  NFA nfa{combined_states, new_accepting_ids, left.getStartStateID()};

  for (const State &state : left.getStates()) {
    StateID from{state.getID()};
    for (Symbol symbol : left.getSymbols(from)) {
      for (StateID to : left.getNextStates(from, symbol)) {
        nfa.addTransition(from, symbol, to);
      }
    }
    for (StateID to : left.getEpsilonNextStates(from)) {
      nfa.addEpsilonTransition(from, to);
    }
  }

  for (const State &state : right.getStates()) {
    StateID from{state.getID()};
    StateID original_from{from - left_size};

    for (Symbol symbol : right.getSymbols(original_from)) {
      for (StateID to : right.getNextStates(original_from, symbol)) {
        nfa.addTransition(from, symbol, left_size + to);
      }
    }
    for (StateID to : right.getEpsilonNextStates(original_from)) {
      nfa.addEpsilonTransition(from, left_size + to);
    }
  }

  for (StateID accepting : left.getAcceptingStateIDs()) {
    nfa.addEpsilonTransition(accepting, left_size + right.getStartStateID());
  }

  return nfa;
}