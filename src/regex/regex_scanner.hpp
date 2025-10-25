#pragma once

#include "../common/types.hpp"
#include "regex_token.hpp"

/**
 * Lexical Grammar:
 * ALTERNATION ::= \|
 * DOT ::= \.
 * STAR ::= \*
 * PLUS ::= \+
 * QUESTION ::= \?
 * LEFT_PAREN ::= \(
 * RIGHT_PAREN ::= \)
 * LEFT_BRACKET ::= \[
 * RIGHT_BRACKET ::= \]
 * LEFT_BRACE ::= \{
 * RIGHT_BRACE ::= \}
 * CARET ::= \^
 * BACKSLASH ::= \\
 * HYPHEN ::= \-
 * COMMA ::= \,
 * CHARACTER ::= .
 * ESCAPED_CHAR ::= BACKSLASH CHARACTER
 * END_OF_INPUT ::= "\0"
 */
class RegexScanner {
private:
  String regex_;
  Index position_ = 0;

  char advance();
  char peek() const;
  bool isAtEnd() const;

public:
  RegexScanner(const String &regex) : regex_(regex) {}

  RegexToken getNextToken();
};