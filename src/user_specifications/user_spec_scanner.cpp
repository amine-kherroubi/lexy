#include "../../include/user_specifications/user_spec_scanner.hpp"
#include <cctype>
#include <stdexcept>

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

UserSpecToken UserSpecScanner::scanUserTokenType() {
  String token_type;
  token_type.push_back(specifications_[position_ - 1]);

  while (!isAtEnd()) {
    char next = peek();
    if (std::isupper(static_cast<unsigned char>(next)) || next == '_') {
      token_type.push_back(advance());
    } else {
      break;
    }
  }

  return {UserSpecTokenType::TOKEN_TYPE, token_type};
}

UserSpecToken UserSpecScanner::scanDefinitionSymbol() {
  if (!isAtEnd() && peek() == ':') {
    advance();
    if (!isAtEnd() && peek() == '=') {
      advance();
      return {UserSpecTokenType::DEFINITION_SYMBOL, "::="};
    }
  }
  throw std::runtime_error("Invalid character sequence starting with ':'");
}

UserSpecToken UserSpecScanner::scanUserRegex() {
  String regex;
  while (!isAtEnd()) {
    char next = advance();
    if (next == '\\') {
      if (isAtEnd())
        throw std::runtime_error("Unterminated regex: escape at end of input");
      char escaped = advance();
      regex.push_back('\\');
      regex.push_back(escaped);
    } else if (next == '"') {
      return {UserSpecTokenType::REGEX, regex};
    } else {
      regex.push_back(next);
    }
  }
  throw std::runtime_error("Unterminated regex: missing closing quote");
}

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

  if (current == '"')
    return scanUserRegex();

  if (std::isupper(static_cast<unsigned char>(current)) || current == '_')
    return scanUserTokenType();

  if (current == ':')
    return scanDefinitionSymbol();

  throw std::runtime_error(String("Unexpected character: '") + current + "'");
}