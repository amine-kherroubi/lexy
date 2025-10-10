#pragma once
#include "NFA.h"
#include <string>

class RegexToNFA {
public:
  static NFA convert(const std::string &regex);

private:
  static NFA buildForSymbol(Symbol c);
  static NFA concatenate(NFA &first_nfa, NFA &second_nfa);
  static NFA alternate(NFA &first_nfa, NFA &second_nfa);
  static NFA kleeneStar(NFA &nfa);
};
