#include "regex_scanner.hpp"

char RegexScanner::advance() {
  if (isAtEnd())
    return '\0';
  return regex_[position_++];
}

char RegexScanner::peek() const {
  if (isAtEnd())
    return '\0';
  return regex_[position_];
}

bool RegexScanner::isAtEnd() const { return position_ >= regex_.size(); }

RegexToken RegexScanner::getNextToken() {
  if (isAtEnd())
    return {RegexTokenType::END_OF_INPUT, ""};

  char current = advance();

  switch (current) {
  case '|':
    return {RegexTokenType::ALTERNATION, String(1, current)};
  case '.':
    return {RegexTokenType::DOT, String(1, current)};
  case '*':
    return {RegexTokenType::STAR, String(1, current)};
  case '+':
    return {RegexTokenType::PLUS, String(1, current)};
  case '?':
    return {RegexTokenType::QUESTION, String(1, current)};
  case '(':
    return {RegexTokenType::LEFT_PAREN, String(1, current)};
  case ')':
    return {RegexTokenType::RIGHT_PAREN, String(1, current)};
  case '[':
    return {RegexTokenType::LEFT_BRACKET, String(1, current)};
  case ']':
    return {RegexTokenType::RIGHT_BRACKET, String(1, current)};
  case '{':
    return {RegexTokenType::LEFT_BRACE, String(1, current)};
  case '}':
    return {RegexTokenType::RIGHT_BRACE, String(1, current)};
  case '^':
    return {RegexTokenType::CARET, String(1, current)};
  case '-':
    return {RegexTokenType::HYPHEN, String(1, current)};
  case ',':
    return {RegexTokenType::COMMA, String(1, current)};
  case '\\':
    if (isAtEnd())
      return {RegexTokenType::INVALID_TOKEN, String(1, current)};
    else {
      char escaped = advance();
      return {RegexTokenType::ESCAPED_CHAR,
              String(1, '\\') + String(1, escaped)};
    }
  default:
    return {RegexTokenType::CHARACTER, String(1, current)};
  }
}