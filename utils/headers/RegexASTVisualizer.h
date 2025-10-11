#pragma once

#include "../../global/types.h"
#include "../../regex/headers/RegexAST.h"
#include <fstream>
#include <iostream>
#include <sstream>

class RegexASTVisualizer {
public:
  static void visualizeAST(const RegexASTNode *root, const String &dot_path,
                           const String &image_path);

private:
  static int nodeCounter;

  static String escapeLabel(const String &label);
  static void generateDotFile(const String &dot_content,
                              const String &filename);
  static void renderDotFile(const String &dot_filename,
                            const String &image_filename,
                            const String &output_format = "png");

  static int visitNode(const RegexASTNode *node, std::ostringstream &dot);
  static int visitChar(const CharNode *node, std::ostringstream &dot);
  static int visitDot(const DotNode *node, std::ostringstream &dot);
  static int visitCharSet(const CharSetNode *node, std::ostringstream &dot);
  static int visitConcat(const ConcatNode *node, std::ostringstream &dot);
  static int visitAlt(const AltNode *node, std::ostringstream &dot);
  static int visitStar(const StarNode *node, std::ostringstream &dot);
  static int visitPlus(const PlusNode *node, std::ostringstream &dot);
  static int visitQuestion(const QuestionNode *node, std::ostringstream &dot);
  static int visitRange(const RangeNode *node, std::ostringstream &dot);
};