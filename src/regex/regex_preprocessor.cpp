#include "regex_preprocessor.hpp"

bool RegexPreprocessor::isOperator(char c) {
  return c == '|' || c == '.' || c == '?' || c == '+' || c == '*';
}

bool RegexPreprocessor::isOperand(char c) {
  return !isOperator(c) && c != '(' && c != ')';
}

int RegexPreprocessor::precedence(char op) {
  switch (op) {
  case '*':
    return 5;
  case '+':
    return 4;
  case '?':
    return 3;
  case '.':
    return 2;
  case '|':
    return 1;
  default:
    return 0;
  }
}

bool RegexPreprocessor::isRightAssociative(char op) {
  return op == '?' or op == '+' or op == '*';
}
bool RegexPreprocessor::isLeftAssociative(char op) {
  return !isRightAssociative(op);
}

String RegexPreprocessor::addConcatenationOperators(const String &regex) {
  String result;
  for (Index i = 0; i < regex.length(); i++) {
    char current{regex[i]};
    result += current;
    if (i + 1 < regex.length()) {
      char next{regex[i + 1]};
      bool is_concat{(isOperand(current) || current == '?' || current == '+' ||
                      current == '*' || current == ')') &&
                     (isOperand(next) || next == '(')};
      if (is_concat) {
        result += '.';
      }
    }
  }
  return result;
}

String RegexPreprocessor::shuntingYard(const String &regex) {
  String output;
  Stack<char> stack;
  for (char c : regex) {
    if (isOperand(c)) {
      output += c;
    } else if (isOperator(c)) {
      while (
          !stack.empty() && isOperator(stack.top()) &&
          ((isLeftAssociative(c) && precedence(c) <= precedence(stack.top())) ||
           (!isLeftAssociative(c) &&
            precedence(c) < precedence(stack.top())))) {
        output += stack.top();
        stack.pop();
      }
      stack.push(c);
    } else if (c == '(') {
      stack.push(c);
    } else if (c == ')') {
      while (!stack.empty() && stack.top() != '(') {
        output += stack.top();
        stack.pop();
      }
      if (!stack.empty())
        stack.pop();
    }
  }
  while (!stack.empty()) {
    output += stack.top();
    stack.pop();
  }
  return output;
}

String RegexPreprocessor::preprocess(const String &regex) {
  return shuntingYard(addConcatenationOperators(regex));
}