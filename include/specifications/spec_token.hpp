#pragma once

#include "../core/types.hpp"

enum class SpecTokenType {
  TOKEN_TYPE,
  DEFINITION_SYMBOL,
  REGEX,
  NEWLINE,
  END_OF_INPUT,
  INVALID_TOKEN,
};

class SpecToken {
private:
  SpecTokenType type_;
  Lexeme lexeme_;

public:
  SpecToken(SpecTokenType type, Lexeme lexeme) : type_(type), lexeme_(lexeme) {}

  SpecTokenType getType() const { return type_; }
  Lexeme getLexeme() const { return lexeme_; }
};