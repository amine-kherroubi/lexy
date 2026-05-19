#pragma once

#include "../common/types.hpp"
#include "user_spec_scanner.hpp"
#include "user_spec_token.hpp"

/**
 * Syntax Grammar:
 *
 * specifications ::= specification (NEWLINE specification)* END_OF_INPUT
 * specification ::= TOKEN_TYPE DEFINITION_SYMBOL REGEX
 *
 * Returns token definitions in declaration order. Order is significant:
 * when two patterns match the same string, the one declared first wins.
 */
class UserSpecParser {
private:
  UserSpecScanner &scanner_;
  UserSpecToken current_token_;

  void consume(UserSpecTokenType);
  Pair<String, String> parse_specification();

public:
  UserSpecParser(UserSpecScanner &scanner)
      : scanner_(scanner), current_token_(scanner.getNextToken()) {}

  // Returns pairs in declaration order (not alphabetically sorted).
  Vector<Pair<String, String>> parse();
};
