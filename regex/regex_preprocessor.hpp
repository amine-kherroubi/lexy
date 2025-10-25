#pragma once

#include "../common/types.hpp"

class RegexPreprocessor {
public:
  static String preprocess(const String &);

private:
  static String addConcatenationOperators(const String &);
  static String shuntingYard(const String &);

  static bool isOperator(char);
  static bool isOperand(char);
  static int precedence(char);
  static bool isRightAssociative(char);
  static bool isLeftAssociative(char);
};
