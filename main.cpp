#include "include/automata/dfa.hpp"
#include "include/automata/dfa_minimizer.hpp"
#include "include/automata/nfa_determinizer.hpp"
#include "include/automata/thompson_construction.hpp"
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

int main() {
  File spec_file("tokens.spec");
  if (!spec_file.is_open()) {
    cerr << "Failed to open 'tokens.spec' for reading" << endl;
    return -1;
  }

  String specifications((istreambuf_iterator<char>(spec_file)),
                        istreambuf_iterator<char>());

  UserSpecScanner user_spec_scanner(specifications);
  UserSpecParser user_spec_parser(user_spec_scanner);
  UnorderedMap<String, String> user_token_types = user_spec_parser.parse();

  UnorderedMap<String, NFA> nfas = {};
  for (auto &[token_type, regex] : user_token_types) {
    RegexScanner regex_scanner(regex);
    RegexParser regex_parser(regex_scanner);
    Pointer<RegexASTNode> regex_ast = regex_parser.parse();
    NFA nfa = RegexASTToNFA::convert(regex_ast);
    nfas.insert({token_type, nfa});
  }

  NFA merged_nfa = ThompsonConstruction::mergeAll(const Vector<NFA> &)
}