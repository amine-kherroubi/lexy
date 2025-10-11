#pragma once

#include "../core/types.hpp"
#include "../regex/ast.hpp"

class RegexASTVisualizer {
public:
  static void visualizeAST(const RegexASTNode *, const String &,
                           const String &);

private:
  static int nodeCounter;

  static String escapeLabel(const String &);
  static void generateDotFile(const String &, const String &);
  static void renderDotFile(const String &, const String &,
                            const String & = "png");

  static int visitNode(const RegexASTNode *, std::ostringstream &);
  static int visitChar(const CharNode *, std::ostringstream &);
  static int visitDot(const DotNode *, std::ostringstream &);
  static int visitCharSet(const CharSetNode *, std::ostringstream &);
  static int visitConcat(const ConcatNode *, std::ostringstream &);
  static int visitAlt(const AltNode *, std::ostringstream &);
  static int visitStar(const StarNode *, std::ostringstream &);
  static int visitPlus(const PlusNode *, std::ostringstream &);
  static int visitQuestion(const QuestionNode *, std::ostringstream &);
  static int visitRange(const RangeNode *, std::ostringstream &);
};