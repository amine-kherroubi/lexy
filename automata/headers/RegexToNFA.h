#pragma once

#include "../../global/types.h"
#include "NFA.h"

class RegexToNFA {
public:
  static NFA convert(const String &regex);

private:
  static NFA buildForSymbol(Symbol c);
  static NFA concatenate(const NFA &first_nfa, const NFA &second_nfa);
  static NFA alternate(const NFA &first_nfa, const NFA &second_nfa);
  static NFA kleeneStar(const NFA &nfa);
};