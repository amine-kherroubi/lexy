#include "../../include/regex/ast_to_nfa.hpp"
#include <stdexcept>

NFA RegexASTToNFA::convert(const RegexASTNode *root) {
  if (!root) {
    throw std::runtime_error("Cannot convert null AST to NFA");
  }
  return visit(root);
}

NFA RegexASTToNFA::visit(const RegexASTNode *node) {
  if (!node) {
    throw std::runtime_error("Null node encountered during AST traversal");
  }

  // Try each node type using dynamic_cast
  if (auto charNode = dynamic_cast<const CharNode *>(node)) {
    return visitChar(charNode);
  } else if (auto dotNode = dynamic_cast<const DotNode *>(node)) {
    return visitDot(dotNode);
  } else if (auto charSetNode = dynamic_cast<const CharSetNode *>(node)) {
    return visitCharSet(charSetNode);
  } else if (auto concatNode = dynamic_cast<const ConcatNode *>(node)) {
    return visitConcat(concatNode);
  } else if (auto altNode = dynamic_cast<const AltNode *>(node)) {
    return visitAlt(altNode);
  } else if (auto starNode = dynamic_cast<const StarNode *>(node)) {
    return visitStar(starNode);
  } else if (auto plusNode = dynamic_cast<const PlusNode *>(node)) {
    return visitPlus(plusNode);
  } else if (auto questionNode = dynamic_cast<const QuestionNode *>(node)) {
    return visitQuestion(questionNode);
  } else if (auto rangeNode = dynamic_cast<const RangeNode *>(node)) {
    return visitRange(rangeNode);
  }

  throw std::runtime_error("Unknown AST node type encountered");
}

NFA RegexASTToNFA::visitChar(const CharNode *node) {
  return buildForSymbol(node->value);
}

NFA RegexASTToNFA::visitDot(const DotNode *node) {
  // The dot matches any printable ASCII character (32-126)
  NFA result = buildForSymbol(' '); // Start with space

  // Build alternation for all printable characters
  for (char c = '!'; c <= '~'; c++) {
    NFA charNFA = buildForSymbol(c);
    result = alternate(result, charNFA);
  }

  return result;
}

NFA RegexASTToNFA::visitCharSet(const CharSetNode *node) {
  Set<char> matchingChars;

  // Add individual characters
  for (char c : node->chars) {
    matchingChars.insert(c);
  }

  // Add character ranges
  for (const auto &range : node->ranges) {
    for (char c = range.first; c <= range.second; c++) {
      matchingChars.insert(c);
    }
  }

  if (matchingChars.empty()) {
    throw std::runtime_error("Empty character set");
  }

  // If negated, we need all characters EXCEPT these
  if (node->negated) {
    Set<char> negatedChars;
    // Consider printable ASCII range
    for (char c = 32; c <= 126; c++) {
      if (matchingChars.find(c) == matchingChars.end()) {
        negatedChars.insert(c);
      }
    }
    matchingChars = negatedChars;
  }

  // Build NFA as alternation of all matching characters
  auto it = matchingChars.begin();
  NFA result = buildForSymbol(*it);
  ++it;

  for (; it != matchingChars.end(); ++it) {
    NFA charNFA = buildForSymbol(*it);
    result = alternate(result, charNFA);
  }

  return result;
}

NFA RegexASTToNFA::visitConcat(const ConcatNode *node) {
  NFA left = visit(node->left.get());
  NFA right = visit(node->right.get());
  return concatenate(left, right);
}

NFA RegexASTToNFA::visitAlt(const AltNode *node) {
  NFA left = visit(node->left.get());
  NFA right = visit(node->right.get());
  return alternate(left, right);
}

NFA RegexASTToNFA::visitStar(const StarNode *node) {
  NFA child = visit(node->child.get());
  return kleeneStar(child, true); // Allow empty string
}

NFA RegexASTToNFA::visitPlus(const PlusNode *node) {
  NFA child = visit(node->child.get());
  return kleeneStar(child, false); // Don't allow empty string
}

NFA RegexASTToNFA::visitQuestion(const QuestionNode *node) {
  // ? means 0 or 1 times
  NFA child = visit(node->child.get());

  // Make the start state accepting to allow 0 occurrences
  StateID start = child.getStartStateID();
  StateIDs &accepting = child.getAcceptingStateIDs();

  bool startIsAccepting = false;
  for (StateID id : accepting) {
    if (id == start) {
      startIsAccepting = true;
      break;
    }
  }

  if (!startIsAccepting) {
    accepting.push_back(start);
  }

  return child;
}

NFA RegexASTToNFA::visitRange(const RangeNode *node) {
  NFA child = visit(node->child.get());
  int min = node->min;
  int max = node->max;

  if (min < 0) {
    throw std::runtime_error("Invalid range: min cannot be negative");
  }

  if (max != -1 && max < min) {
    throw std::runtime_error("Invalid range: max < min");
  }

  // Build min required repetitions
  if (min == 0) {
    throw std::runtime_error("Range with min=0 should be handled differently");
  }

  NFA result = child;
  for (int i = 1; i < min; i++) {
    NFA copy = child;
    result = concatenate(result, copy);
  }

  // Handle the optional part (from min to max)
  if (max == -1) {
    // Unbounded: add star on top
    NFA copy = child;
    NFA starred = kleeneStar(copy, true);
    result = concatenate(result, starred);
  } else if (max > min) {
    // Bounded: add optional repetitions
    for (int i = min; i < max; i++) {
      NFA copy = child;

      // Make it optional
      StateID start = copy.getStartStateID();
      StateIDs &accepting = copy.getAcceptingStateIDs();

      bool startIsAccepting = false;
      for (StateID id : accepting) {
        if (id == start) {
          startIsAccepting = true;
          break;
        }
      }

      if (!startIsAccepting) {
        accepting.push_back(start);
      }

      result = concatenate(result, copy);
    }
  }

  return result;
}

// Helper methods (same as original RegexToNFA)

NFA RegexASTToNFA::buildForSymbol(Symbol c) {
  const States states{State{0}, State{1}};
  const StateIDs accepting_state_ids{1};
  NFA nfa(Alphabet{c}, states, accepting_state_ids, 0);
  nfa.resizeTransitions(2);
  nfa.addTransition(0, c, 1);
  return nfa;
}

NFA RegexASTToNFA::concatenate(const NFA &first_nfa, const NFA &second_nfa) {
  NFA result = first_nfa;

  Alphabet &result_alphabet = result.getAlphabet();
  const Alphabet &second_alphabet = second_nfa.getAlphabet();
  result_alphabet.insert(second_alphabet.begin(), second_alphabet.end());

  int id_offset = result.getStates().size();
  StateIDs old_accepting_ids = result.getAcceptingStateIDs();

  const States &second_states = second_nfa.getStates();
  for (Index i = 0; i < second_states.size(); i++) {
    State new_state{id_offset + second_states[i].getID()};
    result.getStates().push_back(new_state);
  }

  result.resizeTransitions(result.getStates().size());

  StateIDs &new_accepting_ids = result.getAcceptingStateIDs();
  new_accepting_ids.clear();
  const StateIDs &second_accepting = second_nfa.getAcceptingStateIDs();
  for (StateID id : second_accepting) {
    new_accepting_ids.push_back(id_offset + id);
  }

  for (Index i = 0; i < second_states.size(); i++) {
    StateID original_from = second_states[i].getID();
    StateID new_from = id_offset + original_from;

    Symbols symbols = second_nfa.getSymbols(original_from);
    for (Symbol symbol : symbols) {
      StateIDs targets = second_nfa.getNextStateIDs(original_from, symbol);
      for (StateID to : targets) {
        result.addTransition(new_from, symbol, id_offset + to);
      }
    }

    StateIDs epsilon_targets =
        second_nfa.getEpsilonNextStatesIDs(original_from);
    for (StateID to : epsilon_targets) {
      result.addEpsilonTransition(new_from, id_offset + to);
    }
  }

  StateID second_start = id_offset + second_nfa.getStartStateID();
  for (StateID accepting : old_accepting_ids) {
    result.addEpsilonTransition(accepting, second_start);
  }

  return result;
}

NFA RegexASTToNFA::alternate(const NFA &first_nfa, const NFA &second_nfa) {
  Alphabet merged_alphabet = first_nfa.getAlphabet();
  const Alphabet &second_alphabet = second_nfa.getAlphabet();
  merged_alphabet.insert(second_alphabet.begin(), second_alphabet.end());

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

  StateIDs new_accepting_ids;

  const StateIDs &first_accepting = first_nfa.getAcceptingStateIDs();
  for (StateID id : first_accepting) {
    new_accepting_ids.push_back(id + 1);
  }

  const StateIDs &second_accepting = second_nfa.getAcceptingStateIDs();
  for (StateID id : second_accepting) {
    new_accepting_ids.push_back(second_offset + id);
  }

  NFA result(merged_alphabet, new_states, new_accepting_ids, 0);
  result.resizeTransitions(new_states.size());

  for (Index i = 0; i < first_states.size(); i++) {
    StateID original_from = first_states[i].getID();
    StateID new_from = original_from + 1;

    Symbols symbols = first_nfa.getSymbols(original_from);
    for (Symbol symbol : symbols) {
      StateIDs targets = first_nfa.getNextStateIDs(original_from, symbol);
      for (StateID to : targets) {
        result.addTransition(new_from, symbol, to + 1);
      }
    }

    StateIDs epsilon_targets = first_nfa.getEpsilonNextStatesIDs(original_from);
    for (StateID to : epsilon_targets) {
      result.addEpsilonTransition(new_from, to + 1);
    }
  }

  for (Index i = 0; i < second_states.size(); i++) {
    StateID original_from = second_states[i].getID();
    StateID new_from = second_offset + original_from;

    Symbols symbols = second_nfa.getSymbols(original_from);
    for (Symbol symbol : symbols) {
      StateIDs targets = second_nfa.getNextStateIDs(original_from, symbol);
      for (StateID to : targets) {
        result.addTransition(new_from, symbol, second_offset + to);
      }
    }

    StateIDs epsilon_targets =
        second_nfa.getEpsilonNextStatesIDs(original_from);
    for (StateID to : epsilon_targets) {
      result.addEpsilonTransition(new_from, second_offset + to);
    }
  }

  StateID first_start = 1 + first_nfa.getStartStateID();
  StateID second_start = second_offset + second_nfa.getStartStateID();
  result.addEpsilonTransition(0, first_start);
  result.addEpsilonTransition(0, second_start);

  return result;
}

NFA RegexASTToNFA::kleeneStar(const NFA &nfa, bool allow_empty) {
  NFA result = nfa;

  StateID start_state_id = result.getStartStateID();
  StateIDs &accepting_ids = result.getAcceptingStateIDs();

  bool start_is_accepting = false;
  for (StateID id : accepting_ids) {
    if (id == start_state_id) {
      start_is_accepting = true;
      break;
    }
  }

  if (!start_is_accepting && allow_empty) {
    accepting_ids.push_back(start_state_id);
  }

  for (StateID accepting_id : accepting_ids) {
    if (accepting_id != start_state_id) {
      result.addEpsilonTransition(accepting_id, start_state_id);
    }
  }

  return result;
}