#include "headers/RegexToNFA.h"
#include "headers/RegexPreprocessor.h"
#include <stack>

NFA RegexToNFA::buildForSymbol(Symbol c) {
  const States states{State{0}, State{1}};
  const StateIDs accepting_state_ids{1};
  NFA nfa(Alphabet{c}, states, accepting_state_ids, 0);
  nfa.addTransition(0, c, 1);
  return nfa;
}

NFA &RegexToNFA::concatenate(NFA &first_nfa, NFA &second_nfa) {
  // Merge alphabets
  Alphabet &first_alphabet = first_nfa.getAlphabet();
  const Alphabet &second_alphabet = second_nfa.getAlphabet();
  first_alphabet.insert(second_alphabet.begin(), second_alphabet.end());

  // Calculate offset for second NFA states
  int id_offset = first_nfa.getStates().size();

  // Save original accepting states of first NFA before clearing
  StateIDs old_first_accepting_ids = first_nfa.getAcceptingStateIDs();

  // Update state IDs in second NFA
  States &second_nfa_states = second_nfa.getStates();
  for (State &state : second_nfa_states) {
    state.setID(id_offset + state.getID());
  }

  // Add second NFA states to first NFA
  first_nfa.getStates().insert(first_nfa.getStates().end(),
                               second_nfa_states.begin(),
                               second_nfa_states.end());

  // Resize transition table
  first_nfa.resizeTransitions(first_nfa.getStates().size());

  // Update accepting states
  StateIDs &new_accepting_ids = first_nfa.getAcceptingStateIDs();
  new_accepting_ids.clear();
  for (StateID id : second_nfa.getAcceptingStateIDs()) {
    new_accepting_ids.push_back(id_offset + id);
  }

  // Copy transitions from second NFA with offset
  for (size_t i = 0; i < second_nfa.getStates().size(); i++) {
    StateID original_from = i;
    StateID new_from = id_offset + original_from;

    // Copy regular transitions
    for (Symbol symbol : second_nfa.getSymbols(original_from)) {
      for (StateID to : second_nfa.getNextStateIDs(original_from, symbol)) {
        first_nfa.addTransition(new_from, symbol, id_offset + to);
      }
    }

    // Copy epsilon transitions
    for (StateID to : second_nfa.getEpsilonNextStatesIDs(original_from)) {
      first_nfa.addEpsilonTransition(new_from, id_offset + to);
    }
  }

  // Connect old accepting states of first NFA to start of second NFA
  StateID second_start = id_offset + second_nfa.getStartStateID();
  for (StateID accepting : old_first_accepting_ids) {
    first_nfa.addEpsilonTransition(accepting, second_start);
  }

  return first_nfa;
}

NFA &RegexToNFA::alternate(NFA &first_nfa, NFA &second_nfa) {
  // Merge alphabets
  Alphabet &first_alphabet = first_nfa.getAlphabet();
  const Alphabet &second_alphabet = second_nfa.getAlphabet();
  first_alphabet.insert(second_alphabet.begin(), second_alphabet.end());

  size_t old_first_size = first_nfa.getStates().size();

  // Create new start state
  State new_start{0};

  // Shift first NFA states by 1
  States &first_nfa_states = first_nfa.getStates();
  for (State &state : first_nfa_states) {
    state.setID(1 + state.getID());
  }
  first_nfa_states.insert(first_nfa_states.begin(), new_start);

  // Calculate offset for second NFA
  int id_offset = first_nfa_states.size();

  // Update state IDs in second NFA
  States &second_nfa_states = second_nfa.getStates();
  for (State &state : second_nfa_states) {
    state.setID(id_offset + state.getID());
  }

  // Add second NFA states to first NFA
  first_nfa.getStates().insert(first_nfa.getStates().end(),
                               second_nfa_states.begin(),
                               second_nfa_states.end());

  // Resize and shift transitions
  NondeterministicTransitions old_transitions(old_first_size);
  EpsilonTransitions old_epsilon(old_first_size);

  // Copy old transitions
  for (size_t i = 0; i < old_first_size; i++) {
    StateID old_from = i;

    // Copy regular transitions
    for (Symbol symbol : first_nfa.getSymbols(old_from)) {
      old_transitions[i][symbol] = first_nfa.getNextStateIDs(old_from, symbol);
    }

    // Copy epsilon transitions
    old_epsilon[i] = first_nfa.getEpsilonNextStatesIDs(old_from);
  }

  // Resize transition table
  first_nfa.resizeTransitions(first_nfa.getStates().size());

  // Re-add old transitions with shifted state IDs
  for (size_t i = 0; i < old_first_size; i++) {
    StateID new_from = i + 1; // Shifted by 1

    // Add regular transitions
    for (const auto &[symbol, targets] : old_transitions[i]) {
      for (StateID old_to : targets) {
        first_nfa.addTransition(new_from, symbol, old_to + 1);
      }
    }

    // Add epsilon transitions
    for (StateID old_to : old_epsilon[i]) {
      first_nfa.addEpsilonTransition(new_from, old_to + 1);
    }
  }

  // Update accepting states from first NFA (shifted by 1)
  StateIDs &accepting_ids = first_nfa.getAcceptingStateIDs();
  for (StateID &id : accepting_ids) {
    id += 1;
  }

  // Add accepting states from second NFA (with offset)
  for (StateID id : second_nfa.getAcceptingStateIDs()) {
    accepting_ids.push_back(id_offset + id);
  }

  // Set new start state and connect to both original start states
  StateID old_first_start = 1 + first_nfa.getStartStateID();
  StateID second_start = id_offset + second_nfa.getStartStateID();
  first_nfa.setStartStateID(0);
  first_nfa.addEpsilonTransition(0, old_first_start);
  first_nfa.addEpsilonTransition(0, second_start);

  // Copy transitions from second NFA with id_offset
  for (size_t i = 0; i < second_nfa.getStates().size(); i++) {
    StateID original_from = i;
    StateID new_from = id_offset + original_from;

    // Copy regular transitions
    for (Symbol symbol : second_nfa.getSymbols(original_from)) {
      for (StateID to : second_nfa.getNextStateIDs(original_from, symbol)) {
        first_nfa.addTransition(new_from, symbol, id_offset + to);
      }
    }

    // Copy epsilon transitions
    for (StateID to : second_nfa.getEpsilonNextStatesIDs(original_from)) {
      first_nfa.addEpsilonTransition(new_from, id_offset + to);
    }
  }

  return first_nfa;
}

NFA &RegexToNFA::kleeneStar(NFA &nfa) {
  StateID start_state_id = nfa.getStartStateID();
  StateIDs &accepting_ids = nfa.getAcceptingStateIDs();

  // Check if start state is already accepting
  bool start_is_accepting = false;
  for (StateID id : accepting_ids) {
    if (id == start_state_id) {
      start_is_accepting = true;
      break;
    }
  }

  // Make start state accepting (for empty string)
  if (!start_is_accepting) {
    accepting_ids.push_back(start_state_id);
  }

  // Add epsilon transitions from accepting states back to start
  StateIDs accepting_copy = accepting_ids;
  for (StateID accepting_id : accepting_copy) {
    if (accepting_id != start_state_id) {
      nfa.addEpsilonTransition(accepting_id, start_state_id);
    }
  }

  return nfa;
}

NFA RegexToNFA::convert(const String &regex) {
  String postfix = RegexPreprocessor::preprocess(regex);
  std::stack<NFA> nfa_stack;

  for (char c : postfix) {
    if (c == '.') {
      NFA second = std::move(nfa_stack.top());
      nfa_stack.pop();
      NFA first = std::move(nfa_stack.top());
      nfa_stack.pop();
      concatenate(first, second);
      nfa_stack.push(std::move(first));
    } else if (c == '|') {
      NFA second = std::move(nfa_stack.top());
      nfa_stack.pop();
      NFA first = std::move(nfa_stack.top());
      nfa_stack.pop();
      alternate(first, second);
      nfa_stack.push(std::move(first));
    } else if (c == '*') {
      NFA nfa = std::move(nfa_stack.top());
      nfa_stack.pop();
      kleeneStar(nfa);
      nfa_stack.push(std::move(nfa));
    } else {
      // Symbol
      nfa_stack.push(buildForSymbol(c));
    }
  }

  return std::move(nfa_stack.top());
}