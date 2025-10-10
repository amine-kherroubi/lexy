#include "headers/RegexToNFA.h"

StateID RegexToNFA::nextID = 0;

void RegexToNFA::resetIDs() { nextID = 0; }

NFA RegexToNFA::buildForSymbol(Symbol c) {
  resetIDs();
  const StateID start_state_id{nextID++};
  const StateID accept_state_id{nextID++};
  const States states{State{start_state_id}, State{accept_state_id}};
  const StateIDs accepting_state_ids{accept_state_id};

  NFA nfa{states, accepting_state_ids, start_state_id};
  nfa.addTransition(start_state_id, c, accept_state_id);
  return nfa;
}

NFA RegexToNFA::concatenate(const NFA &left, const NFA &right) {
  resetIDs();
  States combined_states{left.getStates()};
  int offset = combined_states.length();
  States right_states{};
  for (State &state : right.getStates()) {
    state.setID(offset + nextID++);
  }
  combined_states.insert(combined_states.end(), right_states.begin(),
                         right_states.end());

  NFA nfa{combined_states, right.getAcceptingStateIDs(),
          left.getStartStateID()};

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
    for (Symbol symbol : right.getSymbols(from)) {
      for (StateID to : right.getNextStates(from, symbol)) {
        nfa.addTransition(from, symbol, to);
      }
    }
    for (StateID to : right.getEpsilonNextStates(from)) {
      nfa.addEpsilonTransition(from, to);
    }
  }

  for (StateID accepting : left.getAcceptingStateIDs()) {
    nfa.addEpsilonTransition(accepting, right.getStartStateID());
  }

  return nfa;
}