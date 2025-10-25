#pragma once

#include "../core/types.hpp"
#include "user_spec_scanner.hpp"
#include "user_spec_token.hpp"

/**
 * Syntax Grammar:
 * specifications ::= DOLLAR SCANNER_NAME NEWLINE specification (NEWLINE
 * specification)* END_OF_INPUT
 * specification ::= TOKEN_TYPE DEFINITION_SYMBOL REGEX
 */
class UserSpecParser {
private:
  UserSpecScanner &scanner_;
  UserSpecToken current_token_;

  void consume(UserSpecTokenType);
  String parse_scanner_name();
  Pair<String, String> parse_specification();

public:
  UserSpecParser(UserSpecScanner &scanner)
      : scanner_(scanner), current_token_(scanner.getNextToken()) {}

  Pair<String, UnorderedMap<String, String>> parse();
};