#pragma once

#include "../automata/dfa.hpp"
#include "../core/types.hpp"

class CodeGenerator {
public:
  static void generateScanner(const DFA &, const Vector<String> &,
                              const String &);

private:
  static String generateTransitionTable(const DFA &);
  static String generateAcceptingStates(const DFA &,
                                        const UnorderedMap<String, int> &);
  static String generateTokenNames(const Vector<String> &);
  static String generateScannerClass(const DFA &);
};