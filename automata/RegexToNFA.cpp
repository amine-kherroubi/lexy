#include "headers/RegexToNFA.h"

StateID RegexToNFA::nextId = 0;

void RegexToNFA::resetIds() { nextId = 0; }

NFA RegexToNFA::buildForSymbol(Symbol c) {
  const StateID start_state_id{nextId++};
  const StateID accept_state_id{nextId++};
  const States states{State{start_state_id}, State{accept_state_id}};
  const StateIDs accepting_state_ids{accept_state_id};

  NFA nfa{states, accepting_state_ids, start_state_id};
  nfa.addTransition(start_state_id, c, accept_state_id);
  return nfa;
}

NFA RegexToNFA::concatenate(const NFA &left, const NFA &right) {
  StateID offset = left.getStates().size();

  States states = left.getStates();
  for (auto &s : right.getStates()) {
    states.emplace_back(s.getId() + offset);
  }

  StateIDs accepting;
  for (auto id : right.getAcceptingStateIDs()) {
    accepting.push_back(id + offset);
  }

  NFA result(states, accepting, left.getStartStateID());

  for (StateID i = 0; i < left.getStates().size(); i++) {
    for (Symbol c : left.getSymbols(i)) {
      for (auto t : left.getNextStates(i, c)) {
        result.addTransition(i, c, t);
      }
    }
    for (auto eps : left.getEpsilonNextStates(i)) {
      result.addEpsilonTransition(i, eps);
    }
  }

  for (StateID i = 0; i < right.getStates().size(); i++) {
    for (Symbol c : right.getSymbols(i)) {
      for (auto t : right.getNextStates(i, c)) {
        result.addTransition(i + offset, c, t + offset);
      }
    }
    for (auto eps : right.getEpsilonNextStates(i)) {
      result.addEpsilonTransition(i + offset, eps + offset);
    }
  }

  StateID right_start = right.getStartStateID() + offset;
  for (auto id : left.getAcceptingStateIDs()) {
    result.addEpsilonTransition(id, right_start);
  }

  return result;
}