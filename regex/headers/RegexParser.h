#pragma once

#include "../../global/types.h"
#include "RegexAST.h"
#include "RegexScanner.h"
#include "RegexToken.h"

class RegexParser {
private:
  RegexScanner &scanner;

  void consume(RegexToken) const;

public:
  explicit RegexParser(RegexScanner &scanner) : scanner(scanner) {}

  Pointer<RegexASTNode> parse();
};
