#include "../../include/automata/thompson_construction.hpp"
#include <stdexcept>

NFA ThompsonConstruction::buildForSymbol(Symbol c) {
  const States states{State{0}, State{1}};
  UnorderedMap<StateID, String> accepting_states;
  accepting_states[1] = "";
  NFA nfa(Alphabet{c}, states, accepting_states, 0);
  nfa.resizeTransitions(2);
  nfa.addTransition(0, c, 1);
  return nfa;
}

NFA ThompsonConstruction::buildForCharSet(const Set<char> &chars) {
  if (chars.empty()) {
    throw std::runtime_error("Cannot build NFA for empty character set");
  }

  const States states{State{0}, State{1}};
  UnorderedMap<StateID, String> accepting_states;
  accepting_states[1] = "";

  Alphabet alphabet(chars.begin(), chars.end());

  NFA nfa(alphabet, states, accepting_states, 0);
  nfa.resizeTransitions(2);

  for (char c : chars) {
    nfa.addTransition(0, c, 1);
  }

  return nfa;
}

NFA ThompsonConstruction::concatenate(const NFA &first_nfa,
                                      const NFA &second_nfa) {
  NFA result = first_nfa;

  Alphabet &result_alphabet = result.getAlphabet();
  const Alphabet &second_alphabet = second_nfa.getAlphabet();
  result_alphabet.insert(second_alphabet.begin(), second_alphabet.end());

  int id_offset = result.getStates().size();
  const StateIDs old_accepting_ids = result.getAcceptingStateIDs();

  const States &second_states = second_nfa.getStates();
  for (Index i = 0; i < second_states.size(); i++) {
    State new_state{id_offset + second_states[i].getID()};
    result.getStates().push_back(new_state);
  }

  result.resizeTransitions(result.getStates().size());

  UnorderedMap<StateID, String> &new_accepting_ids =
      result.getAcceptingStateIDsToTokenTypes();
  new_accepting_ids.clear();
  StateIDs second_accepting_ids = second_nfa.getAcceptingStateIDs();
  for (StateID id : second_accepting_ids) {
    String token_type = second_nfa.getTokenType(id);
    new_accepting_ids[id_offset + id] = token_type;
  }

  copyNFAStructure(second_nfa, result, id_offset);

  StateID second_start = id_offset + second_nfa.getStartStateID();
  for (StateID accepting : old_accepting_ids) {
    result.addEpsilonTransition(accepting, second_start);
  }

  return result;
}

NFA ThompsonConstruction::alternate(const NFA &first_nfa,
                                    const NFA &second_nfa) {
  Alphabet merged_alphabet =
      mergeAlphabets(first_nfa.getAlphabet(), second_nfa.getAlphabet());

  States new_states;
  new_states.push_back(State{0});

  const States &first_states = first_nfa.getStates();
  for (Index i = 0; i < first_states.size(); i++) {
    State new_state{1 + first_states[i].getID()};
    new_states.push_back(new_state);
  }

  int second_offset = new_states.size();

  const States &second_states = second_nfa.getStates();
  for (Index i = 0; i < second_states.size(); i++) {
    State new_state{second_offset + second_states[i].getID()};
    new_states.push_back(new_state);
  }

  UnorderedMap<StateID, String> new_accepting_map;

  StateIDs first_accepting_ids = first_nfa.getAcceptingStateIDs();
  for (StateID id : first_accepting_ids) {
    String token_type = first_nfa.getTokenType(id);
    new_accepting_map[id + 1] = token_type;
  }

  StateIDs second_accepting_ids = second_nfa.getAcceptingStateIDs();
  for (StateID id : second_accepting_ids) {
    String token_type = second_nfa.getTokenType(id);
    new_accepting_map[second_offset + id] = token_type;
  }

  NFA result(merged_alphabet, new_states, new_accepting_map, 0);
  result.resizeTransitions(new_states.size());

  copyNFAStructure(first_nfa, result, 1);
  copyNFAStructure(second_nfa, result, second_offset);

  StateID first_start = 1 + first_nfa.getStartStateID();
  StateID second_start = second_offset + second_nfa.getStartStateID();
  result.addEpsilonTransition(0, first_start);
  result.addEpsilonTransition(0, second_start);

  return result;
}

NFA ThompsonConstruction::kleeneStar(const NFA &nfa, bool allow_empty) {
  NFA result = nfa;

  StateID start_state_id = result.getStartStateID();
  UnorderedMap<StateID, String> &accepting_map =
      result.getAcceptingStateIDsToTokenTypes();

  bool start_is_accepting =
      accepting_map.find(start_state_id) != accepting_map.end();

  if (!start_is_accepting && allow_empty) {
    accepting_map[start_state_id] = "";
  }

  const StateIDs accepting_ids = result.getAcceptingStateIDs();
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
  UnorderedMap<StateID, String> &accepting_map =
      result.getAcceptingStateIDsToTokenTypes();

  bool startIsAccepting = accepting_map.find(start) != accepting_map.end();

  if (!startIsAccepting) {
    accepting_map[start] = "";
  }

  return result;
}

NFA ThompsonConstruction::oneOrMore(const NFA &nfa) {
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

  NFA result = nfa;
  for (int i = 1; i < min; i++) {
    NFA copy = nfa;
    result = concatenate(result, copy);
  }

  if (max == -1) {
    NFA copy = nfa;
    NFA starred = kleeneStar(copy, true);
    result = concatenate(result, starred);
  } else if (max > min) {
    for (int i = min; i < max; i++) {
      NFA copy = nfa;
      NFA optionalCopy = optional(copy);
      result = concatenate(result, optionalCopy);
    }
  }

  return result;
}

NFA ThompsonConstruction::mergeAll(const Vector<NFA> &nfas) {
  if (nfas.empty())
    throw std::runtime_error("mergeAll called with empty vector");
  if (nfas.size() == 1)
    return nfas.front();

  Alphabet merged_alphabet;
  size_t total_states = 1;
  for (const auto &nfa : nfas) {
    const Alphabet &nfa_alphabet = nfa.getAlphabet();
    merged_alphabet.insert(nfa_alphabet.begin(), nfa_alphabet.end());
    total_states += nfa.getStates().size();
  }

  States new_states;
  for (size_t i = 0; i < total_states; ++i)
    new_states.push_back(State{static_cast<int>(i)});

  UnorderedMap<StateID, String> new_accepting_map;
  NFA merged(merged_alphabet, new_states, new_accepting_map, 0);
  merged.resizeTransitions(total_states);

  int offset = 1;
  for (const auto &nfa : nfas) {
    ThompsonConstruction::copyNFAStructure(nfa, merged, offset);

    merged.addEpsilonTransition(0, offset + nfa.getStartStateID());

    StateIDs accepting_ids = nfa.getAcceptingStateIDs();
    for (StateID id : accepting_ids) {
      String token_type = nfa.getTokenType(id);
      merged.getAcceptingStateIDsToTokenTypes()[offset + id] = token_type;
    }

    offset += nfa.getStates().size();
  }

  return merged;
}

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

    const Symbols symbols = source.getSymbols(original_from);
    for (Symbol symbol : symbols) {
      const StateIDs targets = source.getNextStateIDs(original_from, symbol);
      for (StateID to : targets) {
        target.addTransition(new_from, symbol, offset + to);
      }
    }

    const StateIDs epsilon_targets =
        source.getEpsilonNextStatesIDs(original_from);
    for (StateID to : epsilon_targets) {
      target.addEpsilonTransition(new_from, offset + to);
    }
  }
}