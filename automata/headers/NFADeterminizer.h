#pragma once

#include "../../global/types.h"
#include "DFA.h"
#include "NFA.h"

class NFADeterminizer {
public:
  static DFA determinize(NFA &nfa);

private:
  static Closure epsilonClosure(NFA &nfa, StateID state);
  static Closure epsilonClosure(NFA &nfa, Superstate &superstate);

  static Superstate move(NFA &nfa, Superstate &superstate, Symbol symbol);

  static bool containsAcceptingState(NFA &nfa, Superstate &superstate);
};