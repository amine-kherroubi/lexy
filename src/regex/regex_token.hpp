#pragma once

#include "../common/types.hpp"

enum class RegexTokenType {
  ALTERNATION,
  DOT,
  STAR,
  PLUS,
  QUESTION,
  LEFT_PAREN,
  RIGHT_PAREN,
  LEFT_BRACKET,
  RIGHT_BRACKET,
  LEFT_BRACE,
  RIGHT_BRACE,
  CARET,
  BACKSLASH,
  HYPHEN,
  COMMA,
  CHARACTER,
  ESCAPED_CHAR,
  END_OF_INPUT,
  INVALID_TOKEN
};

class RegexToken {
private:
  RegexTokenType type_;
  Lexeme lexeme_;

public:
  RegexToken(RegexTokenType type, Lexeme lexeme)
      : type_(type), lexeme_(lexeme) {}

  RegexTokenType getType() const { return type_; }
  Lexeme getLexeme() const { return lexeme_; }
};