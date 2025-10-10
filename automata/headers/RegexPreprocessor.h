#pragma once

#include "../../global/types.h"

class RegexPreprocessor {
public:
  static String preprocess(const String &regex);

private:
  static String addConcatenationOperators(const String &regex);
  static String shuntingYard(const String &regex);

  static bool isOperator(char c);
  static bool isOperand(char c);
  static int precedence(char op);
  static bool isLeftAssociative(char op);
};
