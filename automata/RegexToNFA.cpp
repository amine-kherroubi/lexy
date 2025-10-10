#include "headers/RegexToNFA.h"
#include "headers/Alphabet.h"
#include "headers/FA.h"
#include "headers/NFA.h"
#include "headers/State.h"

NFA RegexToNFA::buildForSymbol(Symbol c) {
  const StateID start_state_id{0};
  const StateID accept_state_id{1};
  const States states{State{start_state_id}, State{accept_state_id}};
  const StateIDs accepting_state_ids{accept_state_id};

  NFA nfa{states, accepting_state_ids, start_state_id};
  nfa.addTransition(start_state_id, c, accept_state_id);
  return nfa;
}

NFA RegexToNFA::concatenate(const NFA &first_nfa, const NFA &second_nfa) {
  States combined_states{first_nfa.getStates()};
  int offset = combined_states.size();

  States second_nfa_states{second_nfa.getStates()};
  for (State &state : second_nfa_states) {
    state.setID(offset + state.getID());
  }
  combined_states.insert(combined_states.end(), second_nfa_states.begin(),
                         second_nfa_states.end());

  StateIDs new_accepting_ids;
  for (StateID old_id : second_nfa.getAcceptingStateIDs()) {
    new_accepting_ids.push_back(offset + old_id);
  }

  NFA resulting_nfa{combined_states, new_accepting_ids, 0};

  for (const State &state : first_nfa.getStates()) {
    StateID original_from{state.getID()};
    StateID new_from{original_from};

    for (Symbol symbol : first_nfa.getSymbols(original_from)) {
      for (StateID to : first_nfa.getNextStates(original_from, symbol)) {
        resulting_nfa.addTransition(new_from, symbol, to);
      }
    }
    for (StateID to : first_nfa.getEpsilonNextStates(original_from)) {
      resulting_nfa.addEpsilonTransition(new_from, to);
    }
  }

  for (const State &state : second_nfa.getStates()) {
    StateID original_from{state.getID()};
    StateID new_from{offset + original_from};

    for (Symbol symbol : second_nfa.getSymbols(original_from)) {
      for (StateID to : second_nfa.getNextStates(original_from, symbol)) {
        resulting_nfa.addTransition(new_from, symbol, offset + to);
      }
    }
    for (StateID to : second_nfa.getEpsilonNextStates(original_from)) {
      resulting_nfa.addEpsilonTransition(new_from, offset + to);
    }
  }

  for (StateID accepting : first_nfa.getAcceptingStateIDs()) {
    resulting_nfa.addEpsilonTransition(accepting,
                                       offset + second_nfa.getStartStateID());
  }

  return resulting_nfa;
}

NFA RegexToNFA::alternate(const NFA &first_nfa, const NFA &second_nfa) {
  State start_state{0};
  States combined_states{start_state};

  States first_nfa_states{first_nfa.getStates()};
  for (State &state : first_nfa_states) {
    state.setID(state.getID() + 1);
  }
  combined_states.insert(combined_states.end(), first_nfa_states.begin(),
                         first_nfa_states.end());

  int offset = combined_states.size();
  States second_nfa_states{second_nfa.getStates()};
  for (State &state : second_nfa_states) {
    state.setID(offset + state.getID());
  }
  combined_states.insert(combined_states.end(), second_nfa_states.begin(),
                         second_nfa_states.end());

  StateIDs new_accepting_ids;
  for (StateID state_id : first_nfa.getAcceptingStateIDs()) {
    new_accepting_ids.push_back(state_id + 1);
  }
  for (StateID state_id : second_nfa.getAcceptingStateIDs()) {
    new_accepting_ids.push_back(offset + state_id);
  }

  NFA resulting_nfa{combined_states, new_accepting_ids, 0};

  resulting_nfa.addEpsilonTransition(0, first_nfa.getStartStateID() + 1);
  resulting_nfa.addEpsilonTransition(0, offset + second_nfa.getStartStateID());

  for (const State &state : first_nfa.getStates()) {
    StateID original_from{state.getID()};
    StateID new_from{original_from + 1};

    for (Symbol symbol : first_nfa.getSymbols(original_from)) {
      for (StateID to : first_nfa.getNextStates(original_from, symbol)) {
        resulting_nfa.addTransition(new_from, symbol, to + 1);
      }
    }
    for (StateID to : first_nfa.getEpsilonNextStates(original_from)) {
      resulting_nfa.addEpsilonTransition(new_from, to + 1);
    }
  }

  for (const State &state : second_nfa.getStates()) {
    StateID original_from{state.getID()};
    StateID new_from{offset + original_from};

    for (Symbol symbol : second_nfa.getSymbols(original_from)) {
      for (StateID to : second_nfa.getNextStates(original_from, symbol)) {
        resulting_nfa.addTransition(new_from, symbol, offset + to);
      }
    }
    for (StateID to : second_nfa.getEpsilonNextStates(original_from)) {
      resulting_nfa.addEpsilonTransition(new_from, offset + to);
    }
  }

  return resulting_nfa;
}

NFA RegexToNFA::kleeneStar(const NFA &nfa) {
  StateIDs new_accepting_ids{nfa.getStartStateID()};
  for (StateID state_id : nfa.getAcceptingStateIDs()) {
    new_accepting_ids.push_back(state_id);
  }

  NFA resulting_nfa{nfa.getStates(), new_accepting_ids, 0};

  for (StateID state_id : nfa.getAcceptingStateIDs()) {
    resulting_nfa.addEpsilonTransition(state_id, 0);
  }

  return resulting_nfa;
}