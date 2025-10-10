#pragma once
#include "DFA.h"
#include "NFA.h"
#include <map>
#include <set>

class NFADeterminizer {
public:
  static DFA determinize(const NFA &nfa);

private:
  static std::set<StateID> epsilonClosure(const NFA &nfa, StateID state);
  static std::set<StateID> epsilonClosure(const NFA &nfa,
                                          const std::set<StateID> &states);
  static std::set<Symbol> collectAlphabet(const NFA &nfa);
  static std::set<StateID> move(const NFA &nfa, const std::set<StateID> &states,
                                Symbol symbol);
  static bool containsAcceptingState(const NFA &nfa,
                                     const std::set<StateID> &states);
};