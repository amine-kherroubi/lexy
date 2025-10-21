#include "../../include/user_specifications/user_spec_parser.hpp"
#include <cctype>
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

Map<String, String> UserSpecParser::parse() {
  Map<String, String> specifications = {parse_specification()};
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
  for (char c : user_token_type) {
    if (!(std::isupper(static_cast<unsigned char>(c)) || c == '_'))
      throw std::runtime_error(
          "Token type can't contain character '" + String(1, c) +
          "', only uppercase English letters and underscores are allowed");
  }
  consume(UserSpecTokenType::CHAR_SEQUENCE);

  if (current_token_.getLexeme() != "::=")
    throw std::runtime_error("Expected definition symbol '::=', got '" +
                             current_token_.getLexeme() + "'");
  consume(UserSpecTokenType::CHAR_SEQUENCE);

  String regex = current_token_.getLexeme();
  consume(UserSpecTokenType::CHAR_SEQUENCE);

  return {user_token_type, regex};
}