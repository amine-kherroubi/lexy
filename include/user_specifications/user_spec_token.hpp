#pragma once

#include "../core/types.hpp"

enum class UserSpecTokenType { CHAR_SEQUENCE, NEWLINE, END_OF_INPUT };

class UserSpecToken {
private:
  UserSpecTokenType type_;
  Lexeme lexeme_;

public:
  UserSpecToken(UserSpecTokenType type, Lexeme lexeme)
      : type_(type), lexeme_(lexeme) {}

  UserSpecTokenType getType() const { return type_; }
  Lexeme getLexeme() const { return lexeme_; }
};