#pragma once
#include <string>

class RegexPreprocessor {
public:
  static std::string preprocess(const std::string &regex);

private:
  static std::string addConcatenationOperators(const std::string &regex);
  static std::string shuntingYard(const std::string &regex);

  // Helper functions
  static bool isOperator(char c);
  static bool isOperand(char c);
  static int precedence(char op);
  static bool isLeftAssociative(char op);
};
