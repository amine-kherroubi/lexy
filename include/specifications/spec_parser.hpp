#pragma once

#include "../core/types.hpp"
#include "spec_scanner.hpp"
#include "spec_token.hpp"

/**
 * Syntax Grammar:
 * specifications ::= specification (NEWLINE specification)* END_OF_INPUT
 * specification ::= TOKEN_TYPE DEFINITION_SYMBOL REGEX
 */
class SpecParser {
private:
  SpecScanner &scanner_;
  SpecToken current_token_;

  void consume(SpecTokenType);

public:
  SpecParser(SpecScanner &scanner)
      : scanner_(scanner), current_token_(SpecTokenType::INVALID_TOKEN, "") {}

  Map<String, String> parse();
};