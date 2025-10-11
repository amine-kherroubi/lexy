#include "../../include/regex/ast_to_nfa.hpp"
#include "../../include/automata/thompson_construction.hpp"
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
  return ThompsonConstruction::buildForSymbol(node->value);
}

NFA RegexASTToNFA::visitDot(const DotNode *node) {
  // The dot matches any printable ASCII character (32-126)
  NFA result = ThompsonConstruction::buildForSymbol(' '); // Start with space

  // Build alternation for all printable characters
  for (char c = '!'; c <= '~'; c++) {
    NFA charNFA = ThompsonConstruction::buildForSymbol(c);
    result = ThompsonConstruction::alternate(result, charNFA);
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
  NFA result = ThompsonConstruction::buildForSymbol(*it);
  ++it;

  for (; it != matchingChars.end(); ++it) {
    NFA charNFA = ThompsonConstruction::buildForSymbol(*it);
    result = ThompsonConstruction::alternate(result, charNFA);
  }

  return result;
}

NFA RegexASTToNFA::visitConcat(const ConcatNode *node) {
  NFA left = visit(node->left.get());
  NFA right = visit(node->right.get());
  return ThompsonConstruction::concatenate(left, right);
}

NFA RegexASTToNFA::visitAlt(const AltNode *node) {
  NFA left = visit(node->left.get());
  NFA right = visit(node->right.get());
  return ThompsonConstruction::alternate(left, right);
}

NFA RegexASTToNFA::visitStar(const StarNode *node) {
  NFA child = visit(node->child.get());
  return ThompsonConstruction::kleeneStar(child, true); // Allow empty string
}

NFA RegexASTToNFA::visitPlus(const PlusNode *node) {
  NFA child = visit(node->child.get());
  return ThompsonConstruction::kleeneStar(child,
                                          false); // Don't allow empty string
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
    result = ThompsonConstruction::concatenate(result, copy);
  }

  // Handle the optional part (from min to max)
  if (max == -1) {
    // Unbounded: add star on top
    NFA copy = child;
    NFA starred = ThompsonConstruction::kleeneStar(copy, true);
    result = ThompsonConstruction::concatenate(result, starred);
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

      result = ThompsonConstruction::concatenate(result, copy);
    }
  }

  return result;
}