#include "../../global/types.h"
#include "RegexToken.h"

class RegexScanner {
private:
  String regex;
  Index position = 0;

  char advance();
  char peek() const;
  bool isAtEnd() const;

public:
  RegexScanner(const String &regex) : regex(regex) {}

  RegexToken getNextToken();
};