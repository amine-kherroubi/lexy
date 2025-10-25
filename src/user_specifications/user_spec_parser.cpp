#include "../../include/user_specifications/user_spec_parser.hpp"
#include <stdexcept>

void UserSpecParser::consume(UserSpecTokenType expected) {
  if (current_token_.getType() != expected)
    throw std::runtime_error(
        "Expected token type " + std::to_string(static_cast<int>(expected)) +
        " but got " +
        std::to_string(static_cast<int>(current_token_.getType())) +
        " with lexeme '" + current_token_.getLexeme() + "'");

  current_token_ = scanner_.getNextToken();
}

Pair<String, UnorderedMap<String, String>> UserSpecParser::parse() {
  String scanner_name = parse_scanner_name();
  UnorderedMap<String, String> specifications = {parse_specification()};
  while (current_token_.getType() == UserSpecTokenType::NEWLINE) {
    consume(UserSpecTokenType::NEWLINE);
    if (current_token_.getType() == UserSpecTokenType::END_OF_INPUT)
      break;
    specifications.insert(parse_specification());
  }

  return {scanner_name, specifications};
}

String UserSpecParser::parse_scanner_name() {
  consume(UserSpecTokenType::DOLLAR);
  String scanner_name = current_token_.getLexeme();
  consume(UserSpecTokenType::NEWLINE);

  return scanner_name;
}

Pair<String, String> UserSpecParser::parse_specification() {
  String user_token_type = current_token_.getLexeme();
  consume(UserSpecTokenType::TOKEN_TYPE);
  consume(UserSpecTokenType::DEFINITION_SYMBOL);
  String regex = current_token_.getLexeme();
  consume(UserSpecTokenType::REGEX);

  return {user_token_type, regex};
}