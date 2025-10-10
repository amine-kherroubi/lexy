#pragma once
#include "DFA.h"
#include "NFA.h"

class DFADeterminizer {
public:
  static DFA determinize(NFA &nfa) { return DFA(); }
};