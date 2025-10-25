#include "include/automata/dfa.hpp"
#include "include/automata/dfa_minimizer.hpp"
#include "include/automata/nfa_determinizer.hpp"
#include "include/automata/thompson_construction.hpp"
#include "include/code_generation/code_generator.hpp"
#include "include/core/helpers.hpp"
#include "include/regex/ast.hpp"
#include "include/regex/ast_to_nfa.hpp"
#include "include/regex/parser.hpp"
#include "include/regex/scanner.hpp"
#include "include/user_specifications/user_spec_parser.hpp"
#include "include/user_specifications/user_spec_scanner.hpp"
#include <iostream>
#include <sys/stat.h>

using namespace std;

// Helper function to create directory if it doesn't exist
void createDirectory(const String &path) {
#ifdef _WIN32
  _mkdir(path.c_str());
#else
  mkdir(path.c_str(), 0755);
#endif
}

// Helper to check .lexy extension
bool hasLexyExtension(const String &filename) {
  const String ext = ".lexy";
  if (filename.size() < ext.size())
    return false;
  return filename.compare(filename.size() - ext.size(), ext.size(), ext) == 0;
}

// Helper to replace extension with .cpp
String replaceExtensionWithCpp(const String &filename) {
  size_t pos = filename.rfind('.');
  if (pos == String::npos)
    return filename + ".cpp";
  return filename.substr(0, pos) + ".cpp";
}

// Extract base filename from path
String getBaseName(const String &path) {
  size_t slash = path.find_last_of("/\\");
  size_t dot = path.find_last_of('.');
  if (dot == String::npos || dot < slash)
    dot = path.size();
  return path.substr(slash == String::npos ? 0 : slash + 1,
                     dot - (slash == String::npos ? 0 : slash + 1));
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    cerr << "Usage: " << argv[0] << " <input_file.lexy>" << endl;
    return -1;
  }

  String input_filename = argv[1];
  if (!hasLexyExtension(input_filename)) {
    cerr << "Error: input file must have .lexy extension" << endl;
    return -1;
  }

  File spec_file(input_filename);
  if (!spec_file.is_open()) {
    cerr << "Failed to open '" << input_filename << "' for reading" << endl;
    return -1;
  }

  String specifications((istreambuf_iterator<char>(spec_file)),
                        istreambuf_iterator<char>());

  UserSpecScanner user_spec_scanner(specifications);
  UserSpecParser user_spec_parser(user_spec_scanner);
  UnorderedMap<String, String> user_token_types = user_spec_parser.parse();

  Vector<NFA> nfas;
  for (const auto &[token_type, regex] : user_token_types) {
    RegexScanner regex_scanner(regex);
    RegexParser regex_parser(regex_scanner);
    Pointer<RegexASTNode> regex_ast = regex_parser.parse();
    NFA nfa = RegexASTToNFA::convert(regex_ast, token_type);
    nfas.push_back(nfa);
  }

  NFA merged_nfa = ThompsonConstruction::mergeAll(nfas);
  DFA dfa = NFADeterminizer::determinize(merged_nfa);
  DFA minimized = DFAMinimizer::minimize(dfa);

  Vector<String> token_types;
  for (const auto &[token_type, regex] : user_token_types)
    token_types.push_back(token_type);

  // Ensure scanners directory exists
  createDirectory("scanners");

  // Output file path
  String base_name = getBaseName(input_filename);
  String output_filename = "scanners/" + base_name + ".cpp";

  CodeGenerator::generateScanner(minimized, token_types, output_filename);

  return 0;
}