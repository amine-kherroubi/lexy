#pragma once
#include "NFA.h"
#include <string>

class RegexToNFA {
public:
  static NFA convert(const std::string &regex);

private:
  static NFA buildForSymbol(Symbol c);
  static NFA concatenate(const NFA &first_nfa, const NFA &second_nfa);
  static NFA alternate(const NFA &first_nfa, const NFA &second_nfa);
  static NFA kleeneStar(const NFA &nfa);
};
