#include "headers/ShuntingYard.h"
#include <stack>

bool ShuntingYard::isOperator(char c) {
  return c == '|' || c == '.' || c == '*';
}

bool ShuntingYard::isOperand(char c) {
  return !isOperator(c) && c != '(' && c != ')';
}

int ShuntingYard::precedence(char op) {
  switch (op) {
  case '*':
    return 3;
  case '.':
    return 2;
  case '|':
    return 1;
  default:
    return 0;
  }
}

bool ShuntingYard::isLeftAssociative(char op) { return op != '*'; }

std::vector<char> ShuntingYard::convertToRPN(const std::string &regex) {
  std::vector<char> output;
  std::stack<char> stack;

  for (char c : regex) {
    if (isOperand(c)) {
      output.push_back(c);
    } else if (isOperator(c)) {
      while (
          !stack.empty() && isOperator(stack.top()) &&
          ((isLeftAssociative(c) && precedence(c) <= precedence(stack.top())) ||
           (!isLeftAssociative(c) &&
            precedence(c) < precedence(stack.top())))) {
        output.push_back(stack.top());
        stack.pop();
      }
      stack.push(c);
    } else if (c == '(') {
      stack.push(c);
    } else if (c == ')') {
      while (!stack.empty() && stack.top() != '(') {
        output.push_back(stack.top());
        stack.pop();
      }
      if (!stack.empty())
        stack.pop();
    }
  }

  while (!stack.empty()) {
    output.push_back(stack.top());
    stack.pop();
  }

  return output;
}
