#include "user_spec_parser.hpp"

void UserSpecParser::consume(UserSpecTokenType expected) {
  if (current_token_.getType() != expected)
    throw std::runtime_error(
        "Expected token type " + std::to_string(static_cast<int>(expected)) +
        " but got " +
        std::to_string(static_cast<int>(current_token_.getType())) +
        " with lexeme '" + current_token_.getLexeme() + "'");

  current_token_ = scanner_.getNextToken();
}

// Returns specifications in declaration order. Using Vector<Pair> instead of
// Map<String,String> (std::map) avoids silent alphabetical reordering, which
// would break the "first match wins" priority rule for overlapping patterns
// (e.g. IDENTIFIER vs RETURN: whichever is declared first should take priority
// when both match, not whichever comes first alphabetically).
Vector<Pair<String, String>> UserSpecParser::parse() {
  Vector<Pair<String, String>> specifications;
  specifications.push_back(parse_specification());

  while (current_token_.getType() == UserSpecTokenType::NEWLINE) {
    consume(UserSpecTokenType::NEWLINE);
    if (current_token_.getType() == UserSpecTokenType::END_OF_INPUT)
      break;
    specifications.push_back(parse_specification());
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
