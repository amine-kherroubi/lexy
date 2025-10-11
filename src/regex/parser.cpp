#include "../../include/regex/parser.hpp"
#include <stdexcept>

using enum RegexTokenType;

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
  // Initialize by reading the first token
  currentToken = scanner.getNextToken();

  Pointer<RegexASTNode> result = parseAlternation();

  if (currentToken.getType() != END_OF_INPUT) {
    throw std::runtime_error("Unexpected tokens after parsing: '" +
                             currentToken.getLexeme() + "'");
  }

  return result;
}

Pointer<RegexASTNode> RegexParser::parseAlternation() {
  Pointer<RegexASTNode> left = parseConcatenation();

  while (currentToken.getType() == ALTERNATION) {
    consume(ALTERNATION);
    Pointer<RegexASTNode> right = parseConcatenation();
    left = std::make_unique<AltNode>(std::move(left), std::move(right));
  }

  return left;
}

Pointer<RegexASTNode> RegexParser::parseConcatenation() {
  Pointer<RegexASTNode> left = parseRepetition();

  // Continue concatenating while we have atoms (but not alternation or closing
  // parens)
  while (currentToken.getType() != END_OF_INPUT &&
         currentToken.getType() != ALTERNATION &&
         currentToken.getType() != RIGHT_PAREN) {

    Pointer<RegexASTNode> right = parseRepetition();
    left = std::make_unique<ConcatNode>(std::move(left), std::move(right));
  }

  return left;
}

Pointer<RegexASTNode> RegexParser::parseRepetition() {
  Pointer<RegexASTNode> atom = parseAtom();

  // Check for quantifiers
  RegexTokenType type = currentToken.getType();

  if (type == STAR) {
    consume(STAR);
    return std::make_unique<StarNode>(std::move(atom));
  } else if (type == PLUS) {
    consume(PLUS);
    return std::make_unique<PlusNode>(std::move(atom));
  } else if (type == QUESTION) {
    consume(QUESTION);
    return std::make_unique<QuestionNode>(std::move(atom));
  } else if (type == LEFT_BRACE) {
    // Range quantifier {n,m}
    consume(LEFT_BRACE);
    int min = parseNumber();
    int max = min;

    if (currentToken.getType() == COMMA) {
      consume(COMMA);
      if (currentToken.getType() == RIGHT_BRACE) {
        // {n,} - n or more times
        max = -1; // -1 indicates unbounded
      } else {
        max = parseNumber();
      }
    }

    consume(RIGHT_BRACE);

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
  RegexTokenType type = currentToken.getType();

  if (type == CHARACTER) {
    char c = currentToken.getLexeme()[0];
    consume(CHARACTER);
    return std::make_unique<CharNode>(c);
  } else if (type == ESCAPED_CHAR) {
    String lexeme = currentToken.getLexeme();
    consume(ESCAPED_CHAR);

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
  } else if (type == DOT) {
    consume(DOT);
    return std::make_unique<DotNode>();
  } else if (type == LEFT_BRACKET) {
    return parseSet();
  } else if (type == LEFT_PAREN) {
    return parseGroup();
  }

  throw std::runtime_error("Expected atom but got token: " +
                           currentToken.getLexeme());
}

Pointer<RegexASTNode> RegexParser::parseSet() {
  consume(LEFT_BRACKET);

  bool negated = false;
  if (currentToken.getType() == CARET) {
    negated = true;
    consume(CARET);
  }

  auto charSet = std::make_unique<CharSetNode>(negated);

  // Parse set items
  while (currentToken.getType() != RIGHT_BRACKET &&
         currentToken.getType() != END_OF_INPUT) {

    char startChar;

    if (currentToken.getType() == CHARACTER) {
      startChar = currentToken.getLexeme()[0];
      consume(CHARACTER);
    } else if (currentToken.getType() == ESCAPED_CHAR) {
      String lexeme = currentToken.getLexeme();
      if (lexeme.length() == 2) {
        startChar = lexeme[1];
        consume(ESCAPED_CHAR);
      } else {
        throw std::runtime_error("Invalid escaped character in set");
      }
    } else {
      throw std::runtime_error("Expected character in set");
    }

    // Check for range (e.g., a-z)
    if (currentToken.getType() == HYPHEN) {
      consume(HYPHEN);

      char endChar;
      if (currentToken.getType() == CHARACTER) {
        endChar = currentToken.getLexeme()[0];
        consume(CHARACTER);
      } else if (currentToken.getType() == ESCAPED_CHAR) {
        String lexeme = currentToken.getLexeme();
        if (lexeme.length() == 2) {
          endChar = lexeme[1];
          consume(ESCAPED_CHAR);
        } else {
          throw std::runtime_error("Invalid escaped character in range");
        }
      } else {
        throw std::runtime_error("Expected character after hyphen in range");
      }

      if (startChar > endChar) {
        throw std::runtime_error("Invalid range: start > end");
      }

      charSet->ranges.push_back({startChar, endChar});
    } else {
      // Single character
      charSet->chars.push_back(startChar);
    }
  }

  consume(RIGHT_BRACKET);
  return charSet;
}

Pointer<RegexASTNode> RegexParser::parseGroup() {
  consume(LEFT_PAREN);
  Pointer<RegexASTNode> inner = parseAlternation();
  consume(RIGHT_PAREN);
  return inner;
}

int RegexParser::parseNumber() {
  if (currentToken.getType() != CHARACTER) {
    throw std::runtime_error("Expected number");
  }

  String numStr;
  while (currentToken.getType() == CHARACTER &&
         std::isdigit(currentToken.getLexeme()[0])) {
    numStr += currentToken.getLexeme();
    consume(CHARACTER);
  }

  if (numStr.empty()) {
    throw std::runtime_error("Expected number");
  }

  return std::stoi(numStr);
}