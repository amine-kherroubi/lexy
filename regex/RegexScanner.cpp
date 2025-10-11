#include "headers/RegexScanner.h"

using enum RegexTokenType;

char RegexScanner::advance() {
  if (isAtEnd())
    return '\0';
  return regex[position++];
}

char RegexScanner::peek() const {
  if (isAtEnd())
    return '\0';
  return regex[position];
}

bool RegexScanner::isAtEnd() const { return position >= regex.size(); }

RegexToken RegexScanner::getNextToken() {
  if (isAtEnd())
    return {END_OF_INPUT, ""};

  char current = advance();

  switch (current) {
  case '|':
    return {ALTERNATION, String(1, current)};
  case '.':
    return {DOT, String(1, current)};
  case '*':
    return {STAR, String(1, current)};
  case '+':
    return {PLUS, String(1, current)};
  case '?':
    return {QUESTION, String(1, current)};
  case '(':
    return {LEFT_PAREN, String(1, current)};
  case ')':
    return {RIGHT_PAREN, String(1, current)};
  case '[':
    return {LEFT_BRACKET, String(1, current)};
  case ']':
    return {RIGHT_BRACKET, String(1, current)};
  case '{':
    return {LEFT_BRACE, String(1, current)};
  case '}':
    return {RIGHT_BRACE, String(1, current)};
  case '^':
    return {CARET, String(1, current)};
  case '-':
    return {HYPHEN, String(1, current)};
  case ',':
    return {COMMA, String(1, current)};
  case '\\':
    if (isAtEnd())
      return {INVALID_TOKEN, String(1, current)};
    else {
      char escaped = advance();
      return {ESCAPED_CHAR, String(1, '\\') + String(1, escaped)};
    }
  default:
    return {CHARACTER, String(1, current)};
  }
}