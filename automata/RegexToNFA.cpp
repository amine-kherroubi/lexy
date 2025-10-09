#include "headers/RegexToNFA.h"
#include <stack>

StateID RegexToNFA::nextId = 0;

void RegexToNFA::resetIds() { nextId = 0; }

bool RegexToNFA::isOperator(char c) { return c == '|' || c == '.' || c == '*'; }

bool RegexToNFA::isOperand(char c) {
  return !isOperator(c) && c != '(' && c != ')';
}

int RegexToNFA::precedence(char op) {
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

bool RegexToNFA::isLeftAssociative(char op) { return op != '*'; }

std::vector<char> RegexToNFA::shuntingYard(const std::string &regex) {
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

NFA RegexToNFA::buildForSymbol(Symbol c) {
  const StateID start_state_id{nextId++};
  const StateID accept_state_id{nextId++};
  const States states{State{start_state_id}, State{accept_state_id}};
  const StateIDs accepting_state_ids{accept_state_id};

  NFA nfa{states, accepting_state_ids, start_state_id};
  nfa.addTransition(start_state_id, c, accept_state_id);
  return nfa;
}

NFA RegexToNFA::concatenate(const NFA &left, const NFA &right) { return NFA(); }