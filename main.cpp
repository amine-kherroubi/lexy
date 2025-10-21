#include "include/automata/dfa.hpp"
#include "include/automata/dfa_minimizer.hpp"
#include "include/automata/nfa_determinizer.hpp"
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
    cerr << "Can't open specification file";
    return -1;
  }

  String specifications((istreambuf_iterator<char>(spec_file)),
                        istreambuf_iterator<char>());

  UserSpecScanner user_spec_scanner(specifications);
  UserSpecParser user_spec_parser(user_spec_scanner);
  Map<String, String> user_token_types = user_spec_parser.parse();

  // Print results
  cout << "Parsed Token Types:\n";
  cout << "-------------------\n";
  for (const auto &[token_name, regex_pattern] : user_token_types) {
    cout << token_name << " : " << regex_pattern << "\n";
  }
  cout << "\nTotal tokens: " << user_token_types.size() << "\n";
}