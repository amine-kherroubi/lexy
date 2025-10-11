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
  Set<char> allPrintable;
  for (char c = 32; c <= 126; c++) {
    allPrintable.insert(c);
  }
  return ThompsonConstruction::buildForCharSet(allPrintable);
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

  return ThompsonConstruction::buildForCharSet(matchingChars);
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
  return ThompsonConstruction::oneOrMore(child);
}

NFA RegexASTToNFA::visitQuestion(const QuestionNode *node) {
  NFA child = visit(node->child.get());
  return ThompsonConstruction::optional(child);
}

NFA RegexASTToNFA::visitRange(const RangeNode *node) {
  NFA child = visit(node->child.get());
  int min = node->min;
  int max = node->max;

  // Handle special cases that can be converted to simpler operations
  if (min == 0 && max == 1) {
    // {0,1} is same as ?
    return ThompsonConstruction::optional(child);
  }

  if (min == 1 && max == -1) {
    // {1,} is same as +
    return ThompsonConstruction::oneOrMore(child);
  }

  if (min == 0 && max == -1) {
    // {0,} is same as *
    return ThompsonConstruction::kleeneStar(child, true);
  }

  // For all other cases, use the repeat method
  if (min == 0) {
    // {0,n} case: build optional repetitions
    NFA result = ThompsonConstruction::optional(child);
    for (int i = 1; i < max; i++) {
      NFA copy = child;
      NFA optionalCopy = ThompsonConstruction::optional(copy);
      result = ThompsonConstruction::concatenate(result, optionalCopy);
    }
    return result;
  }

  // For min >= 1, use the repeat method
  return ThompsonConstruction::repeat(child, min, max);
}