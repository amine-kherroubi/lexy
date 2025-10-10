#pragma once

#include "../../global/types.h"
#include "DFA.h"
#include "NFA.h"
#include <map>
#include <set>

class NFADeterminizer {
public:
  static DFA determinize(const NFA &nfa);

private:
  static Closure epsilonClosure(const NFA &nfa, StateID state);
  static Closure epsilonClosure(const NFA &nfa, const Superstate &superstate);
  static Alphabet collectAlphabet(const NFA &nfa);
  static Superstate move(const NFA &nfa, const Superstate &superstate,
                         Symbol symbol);
  static bool containsAcceptingState(const NFA &nfa,
                                     const Superstate &superstate);
};