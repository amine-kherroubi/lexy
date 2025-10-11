#pragma once

#include "../../automata/headers/DFA.h"
#include "../../automata/headers/NFA.h"
#include "../../global/types.h"
#include <fstream>
#include <iostream>
#include <sstream>

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