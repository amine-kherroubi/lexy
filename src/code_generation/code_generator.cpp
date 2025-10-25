#include "../../include/code_generation/code_generator.hpp"

void CodeGenerator::generateScanner(const DFA &dfa,
                                    const Vector<String> &token_types,
                                    const String &output_filename) {}

String CodeGenerator::generateTransitionTable(const DFA &) {}
String CodeGenerator::generateAcceptingStates(const DFA &) {}
String CodeGenerator::generateTokenNames(const Vector<String> &) {}
String CodeGenerator::generateScannerClass(const DFA &) {}