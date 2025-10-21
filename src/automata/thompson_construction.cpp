#include "../../include/automata/thompson_construction.hpp"
#include <stdexcept>

NFA ThompsonConstruction::buildForSymbol(Symbol c) {
  const States states{State{0}, State{1}};
  const StateIDs accepting_state_ids{1};
  NFA nfa(Alphabet{c}, states, accepting_state_ids, 0);
  nfa.resizeTransitions(2);
  nfa.addTransition(0, c, 1);
  return nfa;
}

NFA ThompsonConstruction::buildForCharSet(const Set<char> &chars) {
  if (chars.empty()) {
    throw std::runtime_error("Cannot build NFA for empty character set");
  }

  // Instead of alternating N NFAs, create one NFA with N parallel transitions
  const States states{State{0}, State{1}};
  const StateIDs accepting_state_ids{1};

  // Convert Set<char> to Alphabet (which is also Set<char>)
  Alphabet alphabet(chars.begin(), chars.end());

  NFA nfa(alphabet, states, accepting_state_ids, 0);
  nfa.resizeTransitions(2);

  // Add a transition for each character in the set
  for (char c : chars) {
    nfa.addTransition(0, c, 1);
  }

  return nfa;
}

NFA ThompsonConstruction::concatenate(const NFA &first_nfa,
                                      const NFA &second_nfa) {
  NFA result = first_nfa;

  // Merge alphabets
  Alphabet &result_alphabet = result.getAlphabet();
  const Alphabet &second_alphabet = second_nfa.getAlphabet();
  result_alphabet.insert(second_alphabet.begin(), second_alphabet.end());

  // Calculate offset for second NFA's state IDs
  int id_offset = result.getStates().size();
  StateIDs old_accepting_ids = result.getAcceptingStateIDs();

  // Add second NFA's states with offset IDs
  const States &second_states = second_nfa.getStates();
  for (Index i = 0; i < second_states.size(); i++) {
    State new_state{id_offset + second_states[i].getID()};
    result.getStates().push_back(new_state);
  }

  result.resizeTransitions(result.getStates().size());

  // Update accepting states to be from second NFA only
  StateIDs &new_accepting_ids = result.getAcceptingStateIDs();
  new_accepting_ids.clear();
  const StateIDs &second_accepting = second_nfa.getAcceptingStateIDs();
  for (StateID id : second_accepting) {
    new_accepting_ids.push_back(id_offset + id);
  }

  // Copy second NFA's transitions with offset
  copyNFAStructure(second_nfa, result, id_offset);

  // Connect first's accepting states to second's start state
  StateID second_start = id_offset + second_nfa.getStartStateID();
  for (StateID accepting : old_accepting_ids) {
    result.addEpsilonTransition(accepting, second_start);
  }

  return result;
}

NFA ThompsonConstruction::alternate(const NFA &first_nfa,
                                    const NFA &second_nfa) {
  // Merge alphabets
  Alphabet merged_alphabet =
      mergeAlphabets(first_nfa.getAlphabet(), second_nfa.getAlphabet());

  // Create new start state (state 0)
  States new_states;
  new_states.push_back(State{0});

  // Add first NFA's states with offset +1
  const States &first_states = first_nfa.getStates();
  for (Index i = 0; i < first_states.size(); i++) {
    State new_state{1 + first_states[i].getID()};
    new_states.push_back(new_state);
  }

  // Calculate offset for second NFA
  int second_offset = new_states.size();

  // Add second NFA's states with appropriate offset
  const States &second_states = second_nfa.getStates();
  for (Index i = 0; i < second_states.size(); i++) {
    State new_state{second_offset + second_states[i].getID()};
    new_states.push_back(new_state);
  }

  // Collect accepting states from both NFAs
  StateIDs new_accepting_ids;

  const StateIDs &first_accepting = first_nfa.getAcceptingStateIDs();
  for (StateID id : first_accepting) {
    new_accepting_ids.push_back(id + 1);
  }

  const StateIDs &second_accepting = second_nfa.getAcceptingStateIDs();
  for (StateID id : second_accepting) {
    new_accepting_ids.push_back(second_offset + id);
  }

  // Create result NFA
  NFA result(merged_alphabet, new_states, new_accepting_ids, 0);
  result.resizeTransitions(new_states.size());

  // Copy first NFA's structure with offset +1
  copyNFAStructure(first_nfa, result, 1);

  // Copy second NFA's structure with offset
  copyNFAStructure(second_nfa, result, second_offset);

  // Add epsilon transitions from new start state to both NFAs
  StateID first_start = 1 + first_nfa.getStartStateID();
  StateID second_start = second_offset + second_nfa.getStartStateID();
  result.addEpsilonTransition(0, first_start);
  result.addEpsilonTransition(0, second_start);

  return result;
}

NFA ThompsonConstruction::kleeneStar(const NFA &nfa, bool allow_empty) {
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

  // If allow_empty and start isn't accepting, make it accepting
  if (!start_is_accepting && allow_empty) {
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

NFA ThompsonConstruction::optional(const NFA &nfa) {
  NFA result = nfa;

  StateID start = result.getStartStateID();
  StateIDs &accepting = result.getAcceptingStateIDs();

  // Check if start is already accepting
  bool startIsAccepting = false;
  for (StateID id : accepting) {
    if (id == start) {
      startIsAccepting = true;
      break;
    }
  }

  // Make start state accepting to allow zero occurrences
  if (!startIsAccepting) {
    accepting.push_back(start);
  }

  return result;
}

NFA ThompsonConstruction::oneOrMore(const NFA &nfa) {
  // a+ is equivalent to: a followed by a*
  return kleeneStar(nfa, false);
}

NFA ThompsonConstruction::repeat(const NFA &nfa, int min, int max) {
  if (min < 0) {
    throw std::runtime_error("Invalid range: min cannot be negative");
  }

  if (max != -1 && max < min) {
    throw std::runtime_error("Invalid range: max < min");
  }

  if (min == 0) {
    throw std::runtime_error("Range with min=0 not supported by repeat(). Use "
                             "optional() or kleeneStar()");
  }

  // Build min required repetitions: NFA^min
  NFA result = nfa;
  for (int i = 1; i < min; i++) {
    NFA copy = nfa;
    result = concatenate(result, copy);
  }

  // Handle the optional/unbounded part
  if (max == -1) {
    // {n,} - Unbounded: add star on top
    NFA copy = nfa;
    NFA starred = kleeneStar(copy, true);
    result = concatenate(result, starred);
  } else if (max > min) {
    // {n,m} - Bounded: add (m-n) optional repetitions
    for (int i = min; i < max; i++) {
      NFA copy = nfa;
      NFA optionalCopy = optional(copy);
      result = concatenate(result, optionalCopy);
    }
  }
  // If max == min, we already have the result

  return result;
}

NFA ThompsonConstruction::mergeAll(const Vector<NFA> &nfas) {
  if (nfas.empty())
    throw std::runtime_error("mergeAll called with empty vector");
  if (nfas.size() == 1)
    return nfas.front();

  // Merge all alphabets
  Alphabet merged_alphabet;
  size_t total_states = 1; // new global start state (ID 0)
  for (const auto &nfa : nfas) {
    merged_alphabet.insert(nfa.getAlphabet().begin(), nfa.getAlphabet().end());
    total_states += nfa.getStates().size();
  }

  // Create new state list
  States new_states;
  for (size_t i = 0; i < total_states; ++i)
    new_states.push_back(State{static_cast<int>(i)});

  // Collect accepting states
  StateIDs new_accepting_ids;
  NFA merged(merged_alphabet, new_states, new_accepting_ids, 0);
  merged.resizeTransitions(total_states);

  // Copy NFAs and connect with epsilon transitions from global start
  int offset = 1;
  for (const auto &nfa : nfas) {
    ThompsonConstruction::copyNFAStructure(nfa, merged, offset);

    // Add epsilon from global start to each NFA's start
    merged.addEpsilonTransition(0, offset + nfa.getStartStateID());

    // Copy accepting states
    const auto &accepting = nfa.getAcceptingStateIDs();
    for (auto id : accepting)
      merged.getAcceptingStateIDs().push_back(offset + id);

    offset += nfa.getStates().size();
  }

  return merged;
}

// Private helper methods

Alphabet ThompsonConstruction::mergeAlphabets(const Alphabet &first,
                                              const Alphabet &second) {
  Alphabet merged = first;
  merged.insert(second.begin(), second.end());
  return merged;
}

void ThompsonConstruction::copyNFAStructure(const NFA &source, NFA &target,
                                            int offset) {
  const States &source_states = source.getStates();

  for (Index i = 0; i < source_states.size(); i++) {
    StateID original_from = source_states[i].getID();
    StateID new_from = offset + original_from;

    // Copy regular transitions
    Symbols symbols = source.getSymbols(original_from);
    for (Symbol symbol : symbols) {
      StateIDs targets = source.getNextStateIDs(original_from, symbol);
      for (StateID to : targets) {
        target.addTransition(new_from, symbol, offset + to);
      }
    }

    // Copy epsilon transitions
    StateIDs epsilon_targets = source.getEpsilonNextStatesIDs(original_from);
    for (StateID to : epsilon_targets) {
      target.addEpsilonTransition(new_from, offset + to);
    }
  }
}