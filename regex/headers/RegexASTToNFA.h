#pragma once

#include "../../automata/headers/NFA.h"
#include "../../global/types.h"
#include "RegexAST.h"

/**
 * @brief Converts a regex AST to an NFA using a tree walker pattern
 *
 * This class traverses the regex AST and constructs an NFA using Thompson's
 * construction. Each AST node type is handled by a corresponding visit method.
 */
class RegexASTToNFA {
public:
  /**
   * @brief Convert a regex AST to an NFA
   * @param root Pointer to the root of the AST
   * @return NFA constructed from the AST
   */
  static NFA convert(const RegexASTNode *root);

private:
  // Visitor methods for each AST node type
  static NFA visitChar(const CharNode *node);
  static NFA visitDot(const DotNode *node);
  static NFA visitCharSet(const CharSetNode *node);
  static NFA visitConcat(const ConcatNode *node);
  static NFA visitAlt(const AltNode *node);
  static NFA visitStar(const StarNode *node);
  static NFA visitPlus(const PlusNode *node);
  static NFA visitQuestion(const QuestionNode *node);
  static NFA visitRange(const RangeNode *node);

  /**
   * @brief Dispatch visitor method based on node type
   * @param node AST node to visit
   * @return NFA constructed from the node
   */
  static NFA visit(const RegexASTNode *node);

  // Thompson's construction helpers
  static NFA buildForSymbol(Symbol c);
  static NFA concatenate(const NFA &first, const NFA &second);
  static NFA alternate(const NFA &first, const NFA &second);
  static NFA kleeneStar(const NFA &nfa, bool allow_empty = true);
};