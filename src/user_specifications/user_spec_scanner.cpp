#include "../../include/user_specifications/user_spec_scanner.hpp"

char UserSpecScanner::advance() {
  if (isAtEnd())
    return '\0';
  return specifications_[position_++];
}

char UserSpecScanner::peek() const {
  if (isAtEnd())
    return '\0';
  return specifications_[position_];
};

bool UserSpecScanner::isAtEnd() const {
  return position_ >= specifications_.size();
};

UserSpecToken UserSpecScanner::getNextToken() {
  char current = advance();
  while (!isAtEnd() && (current == ' ' || current == '\t' || current == '\r' ||
                        current == '\f' || current == '\v')) {
    current = advance();
  }

  if (isAtEnd())
    return {UserSpecTokenType::END_OF_INPUT, ""};

  if (current == '\n')
    return {UserSpecTokenType::NEWLINE, "\n"};

  String char_sequence;
  char_sequence.push_back(current);

  while (!isAtEnd()) {
    char next = peek();
    if (next == ' ' || next == '\t' || next == '\r' || next == '\f' ||
        next == '\v' || next == '\n')
      break;
    char_sequence.push_back(advance());
  }

  return {UserSpecTokenType::CHAR_SEQUENCE, char_sequence};
}