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

NFA RegexToNFA::concatenate(const NFA &left, const NFA &right) { return NFA(); }