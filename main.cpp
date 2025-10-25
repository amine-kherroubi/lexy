#include "src/automata/dfa.hpp"
#include "src/automata/dfa_minimizer.hpp"
#include "src/automata/nfa_determinizer.hpp"
#include "src/automata/thompson_construction.hpp"
#include "src/code_generation/code_generator.hpp"
#include "src/common/helpers.hpp"
#include "src/regex/regex_ast.hpp"
#include "src/regex/regex_ast_to_nfa.hpp"
#include "src/regex/regex_parser.hpp"
#include "src/regex/regex_scanner.hpp"
#include "src/user_specifications/user_spec_parser.hpp"
#include "src/user_specifications/user_spec_scanner.hpp"
#include <iostream>

using namespace std;

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
  spec_file.close();

  UserSpecScanner user_spec_scanner(specifications);
  UserSpecParser user_spec_parser(user_spec_scanner);
  UnorderedMap<String, String> user_token_types = user_spec_parser.parse();

  // Build NFAs in order and track token types in a vector
  Vector<String> token_types;
  Vector<NFA> nfas;

  for (const auto &[token_type, regex] : user_token_types) {
    cout << "Processing token: " << token_type << " with regex: " << regex
         << endl;

    RegexScanner regex_scanner(regex);
    RegexParser regex_parser(regex_scanner);
    Pointer<RegexASTNode> regex_ast = regex_parser.parse();
    NFA nfa = RegexASTToNFA::convert(regex_ast, token_type);

    nfas.push_back(nfa);
    token_types.push_back(token_type);
  }

  cout << "\nBuilding merged automaton..." << endl;
  NFA merged_nfa = ThompsonConstruction::mergeAll(nfas);
  cout << "Merged NFA has " << merged_nfa.getStates().size() << " states"
       << endl;

  cout << "Determinizing..." << endl;
  DFA dfa = NFADeterminizer::determinize(merged_nfa);
  cout << "DFA has " << dfa.getStates().size() << " states" << endl;

  cout << "Minimizing..." << endl;
  DFA minimized = DFAMinimizer::minimize(dfa);
  cout << "Minimized DFA has " << minimized.getStates().size() << " states"
       << endl;

  // Ensure generated directory exists
  createDirectory("generated");
  createDirectory("generated/scanners");

  // Output file path - correctly strips .lexy extension
  String base_name = getBaseName(input_filename);
  String output_filename = "generated/scanners/" + base_name + ".cpp";

  cout << "\nGenerating scanner code to: " << output_filename << endl;
  CodeGenerator::generateScanner(minimized, token_types, output_filename);

  cout << "\nScanner generation complete!" << endl;
  cout << "Token types (in order): ";
  for (Index i = 0; i < token_types.size(); i++) {
    cout << token_types[i];
    if (i < token_types.size() - 1)
      cout << ", ";
  }
  cout << endl;

  return 0;
}