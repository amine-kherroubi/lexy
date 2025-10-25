#pragma once

#include "../common/types.hpp"
#include "nfa.hpp"

class ThompsonConstruction {
public:
  static NFA buildForSymbol(Symbol);
  static NFA buildForCharSet(const Set<char> &);
  static NFA concatenate(const NFA &, const NFA &);
  static NFA alternate(const NFA &, const NFA &);
  static NFA kleeneStar(const NFA &, bool = true);
  static NFA optional(const NFA &);
  static NFA oneOrMore(const NFA &);
  static NFA repeat(const NFA &, int, int);
  static NFA mergeAll(const Vector<NFA> &);

private:
  static Alphabet mergeAlphabets(const Alphabet &, const Alphabet &);
  static void copyNFAStructure(const NFA &, NFA &, int);
};