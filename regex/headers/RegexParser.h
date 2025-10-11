#pragma once

#include "../../global/types.h"
#include "RegexAST.h"
#include "RegexScanner.h"

class RegexParser {
private:
  RegexScanner &scanner;

public:
  explicit RegexParser(RegexScanner &scanner) : scanner(scanner) {}

  Pointer<RegexASTNode> parse();
};
