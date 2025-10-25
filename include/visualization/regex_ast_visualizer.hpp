#pragma once

#include "../core/types.hpp"
#include "../regex/ast.hpp"

class RegexASTVisualizer {
public:
  static void visualizeAST(const RegexASTNode *, const String &,
                           const String &);

private:
  static int node_counter_;

  static String escapeLabel(const String &);
  static void generateDotFile(const String &, const String &);
  static void renderDotFile(const String &, const String &,
                            const String & = "png");

  static int visitNode(const RegexASTNode *, StringStream &);
  static int visitChar(const CharNode *, StringStream &);
  static int visitDot(const DotNode *, StringStream &);
  static int visitCharSet(const CharSetNode *, StringStream &);
  static int visitConcat(const ConcatNode *, StringStream &);
  static int visitAlt(const AltNode *, StringStream &);
  static int visitStar(const StarNode *, StringStream &);
  static int visitPlus(const PlusNode *, StringStream &);
  static int visitQuestion(const QuestionNode *, StringStream &);
  static int visitRange(const RangeNode *, StringStream &);
};