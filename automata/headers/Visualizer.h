#pragma once

#include "../../global/types.h"
#include "DFA.h"
#include "NFA.h"
#include <fstream>
#include <sstream>

class Visualizer {
public:
  static void visualizeNFA(const NFA &nfa, const String &dot_path,
                           const String &image_path);
  static void visualizeDFA(const DFA &dfa, const String &dot_path,
                           const String &image_path);

private:
  static String escapeLabel(Symbol symbol);
  static void generateDotFile(const String &dot_content,
                              const String &filename);
  static void renderDotFile(const String &dot_filename,
                            const String &image_filename,
                            const String &output_format = "png");
};