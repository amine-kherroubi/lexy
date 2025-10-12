#include "../../include/regex/parser.hpp"
#include <stdexcept>

void RegexParser::consume(RegexTokenType expected) {
  if (current_token_.getType() != expected) {
    throw std::runtime_error(
        "Expected token type " + std::to_string(static_cast<int>(expected)) +
        " but got " +
        std::to_string(static_cast<int>(current_token_.getType())) +
        " with lexeme '" + current_token_.getLexeme() + "'");
  }
  current_token_ = scanner_.getNextToken();
}

Pointer<RegexASTNode> RegexParser::parse() {
  // Initialize by reading the first token
  current_token_ = scanner_.getNextToken();

  Pointer<RegexASTNode> result = parseAlternation();

  if (current_token_.getType() != RegexTokenType::END_OF_INPUT) {
    throw std::runtime_error("Unexpected tokens after parsing: '" +
                             current_token_.getLexeme() + "'");
  }

  return result;
}

Pointer<RegexASTNode> RegexParser::parseAlternation() {
  Pointer<RegexASTNode> left = parseConcatenation();

  while (current_token_.getType() == RegexTokenType::ALTERNATION) {
    consume(RegexTokenType::ALTERNATION);
    Pointer<RegexASTNode> right = parseConcatenation();
    left = std::make_unique<AltNode>(std::move(left), std::move(right));
  }

  return left;
}

Pointer<RegexASTNode> RegexParser::parseConcatenation() {
  Pointer<RegexASTNode> left = parseRepetition();

  // Continue concatenating while we have atoms (but not alternation or closing
  // parens)
  while (current_token_.getType() != RegexTokenType::END_OF_INPUT &&
         current_token_.getType() != RegexTokenType::ALTERNATION &&
         current_token_.getType() != RegexTokenType::RIGHT_PAREN) {

    Pointer<RegexASTNode> right = parseRepetition();
    left = std::make_unique<ConcatNode>(std::move(left), std::move(right));
  }

  return left;
}

Pointer<RegexASTNode> RegexParser::parseRepetition() {
  Pointer<RegexASTNode> atom = parseAtom();

  // Check for quantifiers
  RegexTokenType type = current_token_.getType();

  if (type == RegexTokenType::STAR) {
    consume(RegexTokenType::STAR);
    return std::make_unique<StarNode>(std::move(atom));
  } else if (type == RegexTokenType::PLUS) {
    consume(RegexTokenType::PLUS);
    return std::make_unique<PlusNode>(std::move(atom));
  } else if (type == RegexTokenType::QUESTION) {
    consume(RegexTokenType::QUESTION);
    return std::make_unique<QuestionNode>(std::move(atom));
  } else if (type == RegexTokenType::LEFT_BRACE) {
    // Range quantifier {n,m}
    consume(RegexTokenType::LEFT_BRACE);
    int min = parseNumber();
    int max = min;

    if (current_token_.getType() == RegexTokenType::COMMA) {
      consume(RegexTokenType::COMMA);
      if (current_token_.getType() == RegexTokenType::RIGHT_BRACE) {
        // {n,} - n or more times
        max = -1; // -1 indicates unbounded
      } else {
        max = parseNumber();
      }
    }

    consume(RegexTokenType::RIGHT_BRACE);

    if (min == 0 && max == 0) {
      // {0,0} - matches empty string (epsilon)
      throw std::runtime_error("Range quantifier {0,0} is invalid");
    }

    if (min == 0 && max == 1) {
      // {0,1} is same as ?
      return std::make_unique<QuestionNode>(std::move(atom));
    }

    if (min == 1 && max == -1) {
      // {1,} is same as +
      return std::make_unique<PlusNode>(std::move(atom));
    }

    if (min == 0 && max == -1) {
      // {0,} is same as *
      return std::make_unique<StarNode>(std::move(atom));
    }

    // For other cases, create a RangeNode
    return std::make_unique<RangeNode>(std::move(atom), min, max);
  }

  return atom;
}

Pointer<RegexASTNode> RegexParser::parseAtom() {
  RegexTokenType type = current_token_.getType();

  if (type == RegexTokenType::CHARACTER) {
    char c = current_token_.getLexeme()[0];
    consume(RegexTokenType::CHARACTER);
    return std::make_unique<CharNode>(c);
  } else if (type == RegexTokenType::ESCAPED_CHAR) {
    String lexeme = current_token_.getLexeme();
    consume(RegexTokenType::ESCAPED_CHAR);

    // Handle escaped character (lexeme is like "\n" or "\t")
    if (lexeme.length() == 2) {
      char escaped = lexeme[1];

      // Handle special escape sequences
      switch (escaped) {
      case 'n':
        return std::make_unique<CharNode>('\n');
      case 't':
        return std::make_unique<CharNode>('\t');
      case 'r':
        return std::make_unique<CharNode>('\r');
      case '\\':
        return std::make_unique<CharNode>('\\');
      case '|':
        return std::make_unique<CharNode>('|');
      case '*':
        return std::make_unique<CharNode>('*');
      case '+':
        return std::make_unique<CharNode>('+');
      case '?':
        return std::make_unique<CharNode>('?');
      case '(':
        return std::make_unique<CharNode>('(');
      case ')':
        return std::make_unique<CharNode>(')');
      case '[':
        return std::make_unique<CharNode>('[');
      case ']':
        return std::make_unique<CharNode>(']');
      case '{':
        return std::make_unique<CharNode>('{');
      case '}':
        return std::make_unique<CharNode>('}');
      case '.':
        return std::make_unique<CharNode>('.');
      case '^':
        return std::make_unique<CharNode>('^');
      case '-':
        return std::make_unique<CharNode>('-');
      default:
        return std::make_unique<CharNode>(escaped);
      }
    }
    throw std::runtime_error("Invalid escaped character: " + lexeme);
  } else if (type == RegexTokenType::DOT) {
    consume(RegexTokenType::DOT);
    return std::make_unique<DotNode>();
  } else if (type == RegexTokenType::LEFT_BRACKET) {
    return parseSet();
  } else if (type == RegexTokenType::LEFT_PAREN) {
    return parseGroup();
  }

  throw std::runtime_error("Expected atom but got token: " +
                           current_token_.getLexeme());
}

Pointer<RegexASTNode> RegexParser::parseSet() {
  consume(RegexTokenType::LEFT_BRACKET);

  bool negated = false;
  if (current_token_.getType() == RegexTokenType::CARET) {
    negated = true;
    consume(RegexTokenType::CARET);
  }

  auto charSet = std::make_unique<CharSetNode>(negated);

  // Parse set items
  while (current_token_.getType() != RegexTokenType::RIGHT_BRACKET &&
         current_token_.getType() != RegexTokenType::END_OF_INPUT) {

    char startChar;

    if (current_token_.getType() == RegexTokenType::CHARACTER) {
      startChar = current_token_.getLexeme()[0];
      consume(RegexTokenType::CHARACTER);
    } else if (current_token_.getType() == RegexTokenType::ESCAPED_CHAR) {
      String lexeme = current_token_.getLexeme();
      if (lexeme.length() == 2) {
        startChar = lexeme[1];
        consume(RegexTokenType::ESCAPED_CHAR);
      } else {
        throw std::runtime_error("Invalid escaped character in set");
      }
    } else {
      throw std::runtime_error("Expected character in set");
    }

    // Check for range (e.g., a-z)
    if (current_token_.getType() == RegexTokenType::HYPHEN) {
      consume(RegexTokenType::HYPHEN);

      char endChar;
      if (current_token_.getType() == RegexTokenType::CHARACTER) {
        endChar = current_token_.getLexeme()[0];
        consume(RegexTokenType::CHARACTER);
      } else if (current_token_.getType() == RegexTokenType::ESCAPED_CHAR) {
        String lexeme = current_token_.getLexeme();
        if (lexeme.length() == 2) {
          endChar = lexeme[1];
          consume(RegexTokenType::ESCAPED_CHAR);
        } else {
          throw std::runtime_error("Invalid escaped character in range");
        }
      } else {
        throw std::runtime_error("Expected character after hyphen in range");
      }

      if (startChar > endChar) {
        throw std::runtime_error("Invalid range: start > end");
      }

      charSet->ranges_.push_back({startChar, endChar});
    } else {
      // Single character
      charSet->chars_.push_back(startChar);
    }
  }

  consume(RegexTokenType::RIGHT_BRACKET);
  return charSet;
}

Pointer<RegexASTNode> RegexParser::parseGroup() {
  consume(RegexTokenType::LEFT_PAREN);
  Pointer<RegexASTNode> inner = parseAlternation();
  consume(RegexTokenType::RIGHT_PAREN);
  return inner;
}

int RegexParser::parseNumber() {
  if (current_token_.getType() != RegexTokenType::CHARACTER) {
    throw std::runtime_error("Expected number");
  }

  String numStr;
  while (current_token_.getType() == RegexTokenType::CHARACTER &&
         std::isdigit(current_token_.getLexeme()[0])) {
    numStr += current_token_.getLexeme();
    consume(RegexTokenType::CHARACTER);
  }

  if (numStr.empty()) {
    throw std::runtime_error("Expected number");
  }

  return std::stoi(numStr);
}