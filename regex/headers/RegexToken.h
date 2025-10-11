#pragma once

#include "../../global/types.h"

enum class RegexTokenCategory {
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
  DOLLAR,
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
  RegexTokenCategory category;
  Lexeme lexeme;

public:
  RegexToken(RegexTokenCategory category, Lexeme lexeme)
      : category(category), lexeme(lexeme) {}

  RegexTokenCategory getCategory() const { return category; }
  Lexeme getLexeme() const { return lexeme; }
};