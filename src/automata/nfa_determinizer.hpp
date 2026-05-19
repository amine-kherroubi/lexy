#pragma once

#include "../common/types.hpp"
#include "dfa.hpp"
#include "nfa.hpp"

class NFADeterminizer {
public:
  // token_priority maps each token type name to its declaration index (0 =
  // highest priority). When a superstate contains accepting NFA states for
  // multiple token types, the one with the lowest index wins. This implements
  // the standard "first declaration wins" rule for overlapping patterns.
  static DFA determinize(const NFA &,
                         const UnorderedMap<String, int> &token_priority);

private:
  static Closure epsilonClosure(const NFA &, StateID);
  static Closure epsilonClosure(const NFA &, const Superstate &);
  static Superstate move(const NFA &, const Superstate &, Symbol);
  static bool containsAcceptingState(const NFA &, const Superstate &);

  // Returns the token type with the lowest priority index among all accepting
  // NFA states in the superstate. Falls back to the first accepting state found
  // if a token type is not present in token_priority.
  static String
  resolveTokenType(const NFA &, const Superstate &,
                   const UnorderedMap<String, int> &token_priority);
};
