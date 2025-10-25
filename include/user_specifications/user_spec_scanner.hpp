#pragma once

#include "../core/types.hpp"
#include "user_spec_token.hpp"

/**
 * Lexical Grammar:
 * TOKEN_TYPE ::= [A-Z_]+
 * DEFINITION_SYMBOL ::= ::=
 * REGEX ::= "([^"\\]|\\.)*"
 * NEWLINE ::= \\n
 * END-OF-INPUT ::= \\0
 */
class UserSpecScanner {
private:
  String specifications_;
  Index position_ = 0;

  char advance();
  char peek() const;
  bool isAtEnd() const;
  UserSpecToken scanUserTokenType();
  UserSpecToken scanDefinitionSymbol();
  UserSpecToken scanUserRegex();

public:
  UserSpecScanner(const String &specification)
      : specifications_(specification) {}

  UserSpecToken getNextToken();
};