#include "../../global/types.h"
#include "RegexToken.h"

class RegexParser {
private:
  String regex;
  Index position = 0;

public:
  RegexParser(const String &regex) : regex(regex) {}

  RegexToken getNextToken();
};