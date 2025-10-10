#include "headers/RegexToNFA.h"

NFA RegexToNFA::buildForSymbol(Symbol c) {
  const States states{State{0}, State{1}};
  const StateIDs accepting_state_ids{1};

  NFA nfa{states, accepting_state_ids, 0};
  nfa.addTransition(0, c, 1);
  return nfa;
}

NFA &RegexToNFA::concatenate(NFA &first_nfa, NFA &second_nfa) {
  int offset = first_nfa.getStates().size();

  // Offset second_nfa states and add them to first_nfa
  States &second_nfa_states = second_nfa.getStates();
  for (State &state : second_nfa_states) {
    state.setID(offset + state.getID());
  }
  first_nfa.getStates().insert(first_nfa.getStates().end(),
                               second_nfa_states.begin(),
                               second_nfa_states.end());

  // Update accepting states to be only from second_nfa
  StateIDs &new_accepting_ids = first_nfa.getAcceptingStateIDs();
  StateIDs old_accepting_ids = new_accepting_ids; // Save old accepting states
  new_accepting_ids.clear();

  for (StateID old_id : second_nfa.getAcceptingStateIDs()) {
    new_accepting_ids.push_back(offset + old_id);
  }

  // Copy transitions from second_nfa (with offset)
  for (const State &state : second_nfa_states) {
    StateID original_from = state.getID() - offset;
    StateID new_from = state.getID();

    for (Symbol symbol : second_nfa.getSymbols(original_from)) {
      for (StateID to : second_nfa.getNextStates(original_from, symbol)) {
        first_nfa.addTransition(new_from, symbol, offset + to);
      }
    }
    for (StateID to : second_nfa.getEpsilonNextStates(original_from)) {
      first_nfa.addEpsilonTransition(new_from, offset + to);
    }
  }

  // Add epsilon transitions from old accepting states to second_nfa start
  for (StateID accepting : old_accepting_ids) {
    first_nfa.addEpsilonTransition(accepting,
                                   offset + second_nfa.getStartStateID());
  }

  return first_nfa;
}

NFA &RegexToNFA::alternate(NFA &first_nfa, NFA &second_nfa) {
  // Create new start state and add to first_nfa
  State new_start{0};

  // Offset existing first_nfa states by 1
  States &first_nfa_states = first_nfa.getStates();
  for (State &state : first_nfa_states) {
    state.setID(state.getID() + 1);
  }
  first_nfa_states.insert(first_nfa_states.begin(), new_start);

  // Calculate offset for second_nfa
  int offset = first_nfa_states.size();

  // Offset second_nfa states and add them to first_nfa
  States &second_nfa_states = second_nfa.getStates();
  for (State &state : second_nfa_states) {
    state.setID(offset + state.getID());
  }
  first_nfa.getStates().insert(first_nfa.getStates().end(),
                               second_nfa_states.begin(),
                               second_nfa_states.end());

  // Update accepting states: offset first_nfa's and add second_nfa's
  StateIDs &new_accepting_ids = first_nfa.getAcceptingStateIDs();
  for (StateID &state_id : new_accepting_ids) {
    state_id += 1;
  }
  for (StateID old_id : second_nfa.getAcceptingStateIDs()) {
    new_accepting_ids.push_back(offset + old_id);
  }

  // Update start state
  StateID old_first_start = first_nfa.getStartStateID() + 1;
  first_nfa.setStartStateID(0);

  // Add epsilon transitions from new start state
  first_nfa.addEpsilonTransition(0, old_first_start);
  first_nfa.addEpsilonTransition(0, offset + second_nfa.getStartStateID());

  // Copy transitions from second_nfa (with offset)
  for (const State &state : second_nfa_states) {
    StateID original_from = state.getID() - offset;
    StateID new_from = state.getID();

    for (Symbol symbol : second_nfa.getSymbols(original_from)) {
      for (StateID to : second_nfa.getNextStates(original_from, symbol)) {
        first_nfa.addTransition(new_from, symbol, offset + to);
      }
    }
    for (StateID to : second_nfa.getEpsilonNextStates(original_from)) {
      first_nfa.addEpsilonTransition(new_from, offset + to);
    }
  }

  return first_nfa;
}

NFA &RegexToNFA::kleeneStar(NFA &nfa) {
  StateID start_state_id = nfa.getStartStateID();

  // Add start state to accepting states
  StateIDs &accepting_ids = nfa.getAcceptingStateIDs();
  accepting_ids.insert(accepting_ids.begin(), start_state_id);

  // Add epsilon transitions from accepting states back to start
  for (size_t i = 1; i < accepting_ids.size(); i++) {
    nfa.addEpsilonTransition(accepting_ids[i], start_state_id);
  }

  return nfa;
}