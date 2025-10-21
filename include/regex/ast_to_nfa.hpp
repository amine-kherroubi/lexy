#pragma once

#include "../automata/nfa.hpp"
#include "../core/types.hpp"
#include "ast.hpp"

class RegexASTToNFA {
public:
  static NFA convert(const Pointer<RegexASTNode> &, const String &token_type);

private:
  static NFA visitChar(const CharNode *);
  static NFA visitDot(const DotNode *);
  static NFA visitCharSet(const CharSetNode *);
  static NFA visitConcat(const ConcatNode *);
  static NFA visitAlt(const AltNode *);
  static NFA visitStar(const StarNode *);
  static NFA visitPlus(const PlusNode *);
  static NFA visitQuestion(const QuestionNode *);
  static NFA visitRange(const RangeNode *);
  static NFA visit(const RegexASTNode *);
  static void setTokenType(NFA &, const String &token_type);
};