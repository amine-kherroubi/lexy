#pragma once
#include "../core/types.hpp"
#include "ast.hpp"
#include "scanner.hpp"
#include "token.hpp"

/**
 * @file RegexParser.h
 * @brief Parser for regular expressions, builds an AST from tokenized input.
 *
 * Grammar:
 * @code
 * regex ::= alternation
 *
 * alternation ::= concatenation (ALTERNATION concatenation)*
 *
 * concatenation ::= repetition+
 *
 * repetition ::= atom quantifier?
 *
 * quantifier ::= STAR | PLUS | QUESTION | range_quantifier
 *
 * range_quantifier ::= LEFT_BRACE number (COMMA number?)? RIGHT_BRACE
 *
 * number ::= CHARACTER+  // where CHARACTER is a digit
 *
 * atom ::= CHARACTER | ESCAPED_CHAR | DOT | set | group
 *
 * set ::= LEFT_BRACKET CARET? set_item+ RIGHT_BRACKET
 *
 * set_item ::= CHARACTER | ESCAPED_CHAR | CHARACTER HYPHEN CHARACTER
 *
 * group ::= LEFT_PAREN regex RIGHT_BAREN
 * @endcode
 *
 * @note The parser assumes a RegexScanner providing tokenized input.
 */
class RegexParser {
private:
  RegexScanner &scanner;
  RegexToken currentToken;

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
      : scanner(scanner), currentToken(RegexTokenType::INVALID_TOKEN, "") {}

  Pointer<RegexASTNode> parse();
};