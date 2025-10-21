#pragma once

#include "../automata/dfa.hpp"
#include "../automata/nfa.hpp"
#include "../core/types.hpp"

class AutomataVisualizer {
public:
  static void visualizeNFA(const NFA &, const String &, const String &);
  static void visualizeDFA(const DFA &, const String &, const String &);

private:
  static String escapeLabel(Symbol);
  static void generateDotFile(const String &, const String &);
  static void renderDotFile(const String &, const String &,
                            const String & = "png");
};