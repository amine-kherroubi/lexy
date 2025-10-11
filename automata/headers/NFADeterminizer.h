#pragma once

#include "../../global/types.h"
#include "DFA.h"
#include "NFA.h"

class NFADeterminizer {
public:
  static DFA determinize(const NFA &);

private:
  static Closure epsilonClosure(const NFA &, StateID);
  static Closure epsilonClosure(const NFA &, const Superstate &);
  static Superstate move(const NFA &, const Superstate &, Symbol);
  static bool containsAcceptingState(const NFA &, const Superstate &);
};