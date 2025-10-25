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

UnorderedMap<String, String> UserSpecParser::parse() {
  UnorderedMap<String, String> specifications = {parse_specification()};
  while (current_token_.getType() == UserSpecTokenType::NEWLINE) {
    consume(UserSpecTokenType::NEWLINE);
    if (current_token_.getType() == UserSpecTokenType::END_OF_INPUT)
      break;
    specifications.insert(parse_specification());
  }

  return specifications;
}

Pair<String, String> UserSpecParser::parse_specification() {
  String user_token_type = current_token_.getLexeme();
  consume(UserSpecTokenType::TOKEN_TYPE);
  consume(UserSpecTokenType::DEFINITION_SYMBOL);
  String regex = current_token_.getLexeme();
  consume(UserSpecTokenType::REGEX);

  return {user_token_type, regex};
}