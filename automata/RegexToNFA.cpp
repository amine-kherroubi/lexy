#include "headers/RegexToNFA.h"
#include "../regex/headers/RegexPreprocessor.h"

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
  for (Index i = 0; i < second_states.size(); i++) {
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
  for (Index i = 0; i < second_states.size(); i++) {
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
  // Merge alphabets first
  Alphabet merged_alphabet = first_nfa.getAlphabet();
  const Alphabet &second_alphabet = second_nfa.getAlphabet();
  merged_alphabet.insert(second_alphabet.begin(), second_alphabet.end());

  // Create new start state (state 0)
  States new_states;
  new_states.push_back(State{0});

  // Add first NFA states with offset of 1
  const States &first_states = first_nfa.getStates();
  for (Index i = 0; i < first_states.size(); i++) {
    State new_state{1 + first_states[i].getID()};
    new_states.push_back(new_state);
  }

  // Calculate offset for second NFA
  int second_offset = new_states.size();

  // Add second NFA states with offset
  const States &second_states = second_nfa.getStates();
  for (Index i = 0; i < second_states.size(); i++) {
    State new_state{second_offset + second_states[i].getID()};
    new_states.push_back(new_state);
  }

  // Build accepting states
  StateIDs new_accepting_ids;

  // Add first NFA accepting states (shifted by 1)
  const StateIDs &first_accepting = first_nfa.getAcceptingStateIDs();
  for (StateID id : first_accepting) {
    new_accepting_ids.push_back(id + 1);
  }

  // Add second NFA accepting states (with offset)
  const StateIDs &second_accepting = second_nfa.getAcceptingStateIDs();
  for (StateID id : second_accepting) {
    new_accepting_ids.push_back(second_offset + id);
  }

  // Create result NFA with new start state 0
  NFA result(merged_alphabet, new_states, new_accepting_ids, 0);
  result.resizeTransitions(new_states.size());

  // Copy first NFA transitions (with +1 offset)
  for (Index i = 0; i < first_states.size(); i++) {
    StateID original_from = first_states[i].getID();
    StateID new_from = original_from + 1;

    // Copy regular transitions
    Symbols symbols = first_nfa.getSymbols(original_from);
    for (Symbol symbol : symbols) {
      StateIDs targets = first_nfa.getNextStateIDs(original_from, symbol);
      for (StateID to : targets) {
        result.addTransition(new_from, symbol, to + 1);
      }
    }

    // Copy epsilon transitions
    StateIDs epsilon_targets = first_nfa.getEpsilonNextStatesIDs(original_from);
    for (StateID to : epsilon_targets) {
      result.addEpsilonTransition(new_from, to + 1);
    }
  }

  // Copy second NFA transitions (with offset)
  for (Index i = 0; i < second_states.size(); i++) {
    StateID original_from = second_states[i].getID();
    StateID new_from = second_offset + original_from;

    // Copy regular transitions
    Symbols symbols = second_nfa.getSymbols(original_from);
    for (Symbol symbol : symbols) {
      StateIDs targets = second_nfa.getNextStateIDs(original_from, symbol);
      for (StateID to : targets) {
        result.addTransition(new_from, symbol, second_offset + to);
      }
    }

    // Copy epsilon transitions
    StateIDs epsilon_targets =
        second_nfa.getEpsilonNextStatesIDs(original_from);
    for (StateID to : epsilon_targets) {
      result.addEpsilonTransition(new_from, second_offset + to);
    }
  }

  // Connect new start state (0) to both original start states
  StateID first_start = 1 + first_nfa.getStartStateID();
  StateID second_start = second_offset + second_nfa.getStartStateID();
  result.addEpsilonTransition(0, first_start);
  result.addEpsilonTransition(0, second_start);

  return result;
}

NFA RegexToNFA::kleeneStar(const NFA &nfa, bool non_empty) {
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

  // Make start state accepting (for empty string) only if not already
  if (!start_is_accepting and not !non_empty) {
    accepting_ids.push_back(start_state_id);
  }

  // Add epsilon transitions from all accepting states back to start
  for (StateID accepting_id : accepting_ids) {
    if (accepting_id != start_state_id) {
      result.addEpsilonTransition(accepting_id, start_state_id);
    }
  }

  return result;
}

NFA RegexToNFA::convert(const String &regex) {
  String postfix = RegexPreprocessor::preprocess(regex);
  Stack<NFA> nfa_stack;

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

    } else if (c == '?') {
      // Pop the NFA to apply Kleene star
      NFA nfa = nfa_stack.top();
      nfa_stack.pop();

      // Apply Kleene star and push result
      NFA result = kleeneStar(nfa);
      nfa_stack.push(result);

    } else if (c == '+') {
      // Pop the NFA to apply Kleene star
      NFA nfa = nfa_stack.top();
      nfa_stack.pop();

      // Apply Kleene star and push result
      NFA result = kleeneStar(nfa, true);
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