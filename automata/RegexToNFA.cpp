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
  // Create mapping for left NFA state IDs
  std::unordered_map<StateID, StateID> left_id_map;
  for (const State &state : left.getStates()) {
    left_id_map[state.getId()] = nextId++;
  }

  // Create mapping for right NFA state IDs
  std::unordered_map<StateID, StateID> right_id_map;
  for (const State &state : right.getStates()) {
    right_id_map[state.getId()] = nextId++;
  }

  // Combine states from both NFAs with new IDs
  States combined_states{};
  for (const State &state : left.getStates()) {
    combined_states.push_back(State{left_id_map[state.getId()]});
  }
  for (const State &state : right.getStates()) {
    combined_states.push_back(State{right_id_map[state.getId()]});
  }

  // Map accepting states to new IDs
  StateIDs new_accepting_states;
  for (StateID accepting : right.getAcceptingStateIDs()) {
    new_accepting_states.push_back(right_id_map[accepting]);
  }

  NFA nfa{combined_states, new_accepting_states,
          left_id_map[left.getStartStateID()]};

  // Copy left NFA transitions with remapped IDs
  for (const State &state : left.getStates()) {
    StateID old_from{state.getId()};
    StateID new_from{left_id_map[old_from]};

    for (Symbol symbol : left.getSymbols(old_from)) {
      for (StateID old_to : left.getNextStates(old_from, symbol)) {
        nfa.addTransition(new_from, symbol, left_id_map[old_to]);
      }
    }
    for (StateID old_to : left.getEpsilonNextStates(old_from)) {
      nfa.addEpsilonTransition(new_from, left_id_map[old_to]);
    }
  }

  // Copy right NFA transitions with remapped IDs
  for (const State &state : right.getStates()) {
    StateID old_from{state.getId()};
    StateID new_from{right_id_map[old_from]};

    for (Symbol symbol : right.getSymbols(old_from)) {
      for (StateID old_to : right.getNextStates(old_from, symbol)) {
        nfa.addTransition(new_from, symbol, right_id_map[old_to]);
      }
    }
    for (StateID old_to : right.getEpsilonNextStates(old_from)) {
      nfa.addEpsilonTransition(new_from, right_id_map[old_to]);
    }
  }

  // Connect left's accepting states to right's start state
  for (StateID accepting : left.getAcceptingStateIDs()) {
    nfa.addEpsilonTransition(left_id_map[accepting],
                             right_id_map[right.getStartStateID()]);
  }

  return nfa;
}