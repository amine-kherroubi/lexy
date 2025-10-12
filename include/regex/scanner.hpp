#pragma once

#include "../core/types.hpp"
#include "token.hpp"

class RegexScanner {
private:
  String regex_;
  Index position_ = 0;

  char advance();
  char peek() const;
  bool isAtEnd() const;

public:
  RegexScanner(const String &regex) : regex_(regex) {}

  RegexToken getNextToken();
};