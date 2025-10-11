#include "headers/RegexToNFA.h"
#include "headers/RegexPreprocessor.h"
#include <stack>

NFA RegexToNFA::buildForSymbol(Symbol c) {
  const States states{State{0}, State{1}};
  const StateIDs accepting_state_ids{1};
  NFA nfa(Alphabet{c}, states, accepting_state_ids, 0);
  nfa.resizeTransitions(2);
  nfa.addTransition(0, c, 1);
  return nfa;
}

NFA RegexToNFA::concatenate(const NFA &first_nfa, const NFA &second_nfa) {
  // Create a copy to work with
  NFA result = first_nfa;

  // Merge alphabets
  Alphabet &result_alphabet = result.getAlphabet();
  const Alphabet &second_alphabet = second_nfa.getAlphabet();
  result_alphabet.insert(second_alphabet.begin(), second_alphabet.end());

  // Calculate offset for second NFA states
  int id_offset = result.getStates().size();

  // Save original accepting states before clearing
  StateIDs old_accepting_ids = result.getAcceptingStateIDs();

  // Add second NFA states with offset
  const States &second_states = second_nfa.getStates();
  for (size_t i = 0; i < second_states.size(); i++) {
    State new_state{id_offset + second_states[i].getID()};
    result.getStates().push_back(new_state);
  }

  // Resize transition table
  result.resizeTransitions(result.getStates().size());

  // Update accepting states to second NFA's accepting states (with offset)
  StateIDs &new_accepting_ids = result.getAcceptingStateIDs();
  new_accepting_ids.clear();
  const StateIDs &second_accepting = second_nfa.getAcceptingStateIDs();
  for (StateID id : second_accepting) {
    new_accepting_ids.push_back(id_offset + id);
  }

  // Copy transitions from second NFA with offset
  for (size_t i = 0; i < second_states.size(); i++) {
    StateID original_from = second_states[i].getID();
    StateID new_from = id_offset + original_from;

    // Copy regular transitions
    Symbols symbols = second_nfa.getSymbols(original_from);
    for (Symbol symbol : symbols) {
      StateIDs targets = second_nfa.getNextStateIDs(original_from, symbol);
      for (StateID to : targets) {
        result.addTransition(new_from, symbol, id_offset + to);
      }
    }

    // Copy epsilon transitions
    StateIDs epsilon_targets =
        second_nfa.getEpsilonNextStatesIDs(original_from);
    for (StateID to : epsilon_targets) {
      result.addEpsilonTransition(new_from, id_offset + to);
    }
  }

  // Connect old accepting states of first NFA to start of second NFA
  StateID second_start = id_offset + second_nfa.getStartStateID();
  for (StateID accepting : old_accepting_ids) {
    result.addEpsilonTransition(accepting, second_start);
  }

  return result;
}

NFA RegexToNFA::alternate(const NFA &first_nfa, const NFA &second_nfa) {
  // Create new NFA with a new start state (state 0)
  NFA result = first_nfa;

  // Merge alphabets
  Alphabet &result_alphabet = result.getAlphabet();
  const Alphabet &second_alphabet = second_nfa.getAlphabet();
  result_alphabet.insert(second_alphabet.begin(), second_alphabet.end());

  // Shift all first NFA states by 1
  States &result_states = result.getStates();
  for (State &state : result_states) {
    state.setID(1 + state.getID());
  }

  // Add new start state at position 0
  State new_start{0};
  result_states.insert(result_states.begin(), new_start);

  // Calculate offset for second NFA (after first NFA + new start state)
  int id_offset = result_states.size();

  // Add second NFA states with offset
  const States &second_states = second_nfa.getStates();
  for (size_t i = 0; i < second_states.size(); i++) {
    State new_state{id_offset + second_states[i].getID()};
    result_states.push_back(new_state);
  }

  // Save old transitions before resizing
  size_t old_first_size = first_nfa.getStates().size();
  NondeterministicTransitions old_transitions(old_first_size);
  EpsilonTransitions old_epsilon(old_first_size);

  // Copy old first NFA transitions
  for (size_t i = 0; i < old_first_size; i++) {
    StateID old_from = first_nfa.getStates()[i].getID();

    // Copy regular transitions
    Symbols symbols = first_nfa.getSymbols(old_from);
    for (Symbol symbol : symbols) {
      old_transitions[i][symbol] = first_nfa.getNextStateIDs(old_from, symbol);
    }

    // Copy epsilon transitions
    old_epsilon[i] = first_nfa.getEpsilonNextStatesIDs(old_from);
  }

  // Resize transition table for all states
  result.resizeTransitions(result_states.size());

  // Re-add first NFA transitions with shifted state IDs (+1)
  for (size_t i = 0; i < old_first_size; i++) {
    StateID new_from = i + 1;

    // Add regular transitions
    for (const auto &[symbol, targets] : old_transitions[i]) {
      for (StateID old_to : targets) {
        result.addTransition(new_from, symbol, old_to + 1);
      }
    }

    // Add epsilon transitions
    for (StateID old_to : old_epsilon[i]) {
      result.addEpsilonTransition(new_from, old_to + 1);
    }
  }

  // Update accepting states from first NFA (shifted by 1)
  StateIDs &accepting_ids = result.getAcceptingStateIDs();
  for (StateID &id : accepting_ids) {
    id += 1;
  }

  // Add accepting states from second NFA (with offset)
  const StateIDs &second_accepting = second_nfa.getAcceptingStateIDs();
  for (StateID id : second_accepting) {
    accepting_ids.push_back(id_offset + id);
  }

  // Copy transitions from second NFA with offset
  for (size_t i = 0; i < second_states.size(); i++) {
    StateID original_from = second_states[i].getID();
    StateID new_from = id_offset + original_from;

    // Copy regular transitions
    Symbols symbols = second_nfa.getSymbols(original_from);
    for (Symbol symbol : symbols) {
      StateIDs targets = second_nfa.getNextStateIDs(original_from, symbol);
      for (StateID to : targets) {
        result.addTransition(new_from, symbol, id_offset + to);
      }
    }

    // Copy epsilon transitions
    StateIDs epsilon_targets =
        second_nfa.getEpsilonNextStatesIDs(original_from);
    for (StateID to : epsilon_targets) {
      result.addEpsilonTransition(new_from, id_offset + to);
    }
  }

  // Set new start state and connect to both original start states
  StateID old_first_start = 1 + first_nfa.getStartStateID();
  StateID second_start = id_offset + second_nfa.getStartStateID();
  result.setStartStateID(0);
  result.addEpsilonTransition(0, old_first_start);
  result.addEpsilonTransition(0, second_start);

  return result;
}

NFA RegexToNFA::kleeneStar(const NFA &nfa) {
  // Create a copy to work with
  NFA result = nfa;

  StateID start_state_id = result.getStartStateID();
  StateIDs &accepting_ids = result.getAcceptingStateIDs();

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
  // Make a copy to avoid modifying the list we're iterating over
  StateIDs accepting_copy = accepting_ids;
  for (StateID accepting_id : accepting_copy) {
    if (accepting_id != start_state_id) {
      result.addEpsilonTransition(accepting_id, start_state_id);
    }
  }

  return result;
}

NFA RegexToNFA::convert(const String &regex) {
  String postfix = RegexPreprocessor::preprocess(regex);
  std::stack<NFA> nfa_stack;

  for (char c : postfix) {
    if (c == '.') {
      // Pop second, then first (order matters)
      NFA second = nfa_stack.top();
      nfa_stack.pop();
      NFA first = nfa_stack.top();
      nfa_stack.pop();

      // Concatenate and push result
      NFA result = concatenate(first, second);
      nfa_stack.push(result);

    } else if (c == '|') {
      // Pop second, then first (order matters)
      NFA second = nfa_stack.top();
      nfa_stack.pop();
      NFA first = nfa_stack.top();
      nfa_stack.pop();

      // Alternate and push result
      NFA result = alternate(first, second);
      nfa_stack.push(result);

    } else if (c == '*') {
      // Pop the NFA to apply Kleene star
      NFA nfa = nfa_stack.top();
      nfa_stack.pop();

      // Apply Kleene star and push result
      NFA result = kleeneStar(nfa);
      nfa_stack.push(result);

    } else {
      // Regular symbol - build NFA and push
      nfa_stack.push(buildForSymbol(c));
    }
  }

  return nfa_stack.top();
}