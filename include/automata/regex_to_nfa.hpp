#pragma once

#include "../core/types.hpp"
#include "nfa.hpp"

class RegexToNFA {
public:
  static NFA convert(const String &);

private:
  static NFA buildForSymbol(Symbol);
  static NFA concatenate(const NFA &, const NFA &);
  static NFA alternate(const NFA &, const NFA &);
  static NFA kleeneStar(const NFA &, bool = false);
};