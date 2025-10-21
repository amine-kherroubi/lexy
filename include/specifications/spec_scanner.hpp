#pragma once

#include "../core/types.hpp"
#include "spec_token.hpp"

/**
 * Lexical Grammar:
 * TOKEN_TYPE ::= [A-Z_]+
 * DEFINITION_SYMBOL ::= ::=
 * REGEX ::= [^\t\r\n\f\v]+
 * NEWLINE ::= \\n
 * END-OF-INPUT ::= \\0
 */
class SpecScanner {
private:
  String specifications_;
  Index position_ = 0;
  char advance();
  char peek() const;
  bool isAtEnd() const;

public:
  SpecScanner(const String &specification) : specifications_(specification) {}

  SpecToken getNextToken();
};