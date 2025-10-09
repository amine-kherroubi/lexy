#include "NFA.h"
#include <string>

class RegexToNFA {
public:
  static NFA convert(const std::string &regex);
  static void resetIds();

private:
  static StateID nextId;
  static NFA buildForSymbol(Symbol c);
  static NFA concatenate(const NFA &left, const NFA &right);
  static NFA alternate(const NFA &left, const NFA &right);
  static NFA kleeneStar(const NFA &nfa);

  // may later add a tokenizer or shunting-yard algorithm here
};
