#pragma once

#include "../common/types.hpp"
#include "regex_ast.hpp"
#include "regex_scanner.hpp"

/**
 * Syntax Grammar:
 * regex ::= alternation END_OF_INPUT
 * alternation ::= concatenation (ALTERNATION concatenation)*
 * concatenation ::= repetition+
 * repetition ::= atom quantifier?
 * quantifier ::= STAR | PLUS | QUESTION | range_quantifier
 * range_quantifier ::= LEFT_BRACE number (COMMA number?)? RIGHT_BRACE
 * number ::= CHARACTER+ // where CHARACTER is a digit
 * atom ::= CHARACTER | ESCAPED_CHAR | DOT | set | group
 * set ::= LEFT_BRACKET CARET? set_item+ RIGHT_BRACKET
 * set_item ::= CHARACTER | ESCAPED_CHAR | CHARACTER HYPHEN CHARACTER
 * group ::= LEFT_PAREN regex RIGHT_BAREN
 */
class RegexParser {
private:
  RegexScanner &scanner_;
  RegexToken current_token_;

  void consume(RegexTokenType);

  Pointer<RegexASTNode> parseAlternation();
  Pointer<RegexASTNode> parseConcatenation();
  Pointer<RegexASTNode> parseRepetition();
  Pointer<RegexASTNode> parseAtom();
  Pointer<RegexASTNode> parseQuantifier();
  Pointer<RegexASTNode> parseSet();
  Pointer<RegexASTNode> parseGroup();
  int parseNumber();

public:
  RegexParser(RegexScanner &scanner)
      : scanner_(scanner), current_token_(scanner.getNextToken()) {}

  Pointer<RegexASTNode> parse();
};