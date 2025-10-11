#pragma once

#include "../../global/types.h"

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
  RegexTokenType type;
  Lexeme lexeme;

public:
  RegexToken(RegexTokenType type, Lexeme lexeme) : type(type), lexeme(lexeme) {}

  RegexTokenType getType() const { return type; }
  Lexeme getLexeme() const { return lexeme; }
};