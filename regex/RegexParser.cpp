#include "headers/RegexParser.h"
#include "headers/RegexAST.h"
#include <stdexcept>

void RegexParser::consume(RegexTokenType expected) {
  if (currentToken.getType() != expected) {
    throw std::runtime_error(
        "Expected token type " + std::to_string(static_cast<int>(expected)) +
        " but got " + std::to_string(static_cast<int>(currentToken.getType())) +
        " with lexeme '" + currentToken.getLexeme() + "'");
  }
  currentToken = scanner.getNextToken();
}

Pointer<RegexASTNode> RegexParser::parse() {
  Pointer<RegexASTNode> alt_node = parseAlternation();
};

Pointer<AltNode> RegexParser::parseAlternation();
Pointer<RegexASTNode> RegexParser::parseConcatenation();
Pointer<RegexASTNode> RegexParser::parseRepetition();
Pointer<RegexASTNode> RegexParser::parseAtom();
Pointer<RegexASTNode> RegexParser::parseQuantifier();
Pointer<RegexASTNode> RegexParser::parseSet();
Pointer<RegexASTNode> RegexParser::parseGroup();
int parseNumber();

Pointer<RegexASTNode> RegexParser::parse() {
  RegexToken first_token = scanner.getNextToken();
}