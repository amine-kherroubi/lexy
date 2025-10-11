#pragma once

#include "../../global/types.h"
#include "DFA.h"
#include "NFA.h"

class NFADeterminizer {
public:
  static DFA determinize(const NFA &nfa);

private:
  static Closure epsilonClosure(const NFA &nfa, StateID state);
  static Closure epsilonClosure(const NFA &nfa, const Superstate &superstate);
  static Superstate move(const NFA &nfa, const Superstate &superstate,
                         Symbol symbol);
  static bool containsAcceptingState(const NFA &nfa,
                                     const Superstate &superstate);
};