#include "include/automata/dfa_minimizer.hpp"
#include "include/automata/nfa_determinizer.hpp"
#include "include/regex/ast_to_nfa.hpp"
#include "include/regex/parser.hpp"
#include "include/regex/scanner.hpp"
#include "include/utils/automata_visualizer.hpp"
#include "include/utils/regex_ast_visualizer.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <vector>

using namespace std;

// Helper function to create directory if it doesn't exist
void createDirectory(const String &path) {
#ifdef _WIN32
  _mkdir(path.c_str());
#else
  mkdir(path.c_str(), 0755);
#endif
}

int main(int argc, char *argv[]) {
  std::ifstream spec_file("tokens.spec");
  if (!spec_file.is_open()) {
    std::cerr << "Can't open specification file";
    return -1;
  }

  std::vector<std::string> token_specs = {};
  std::string line;
  while (std::getline(spec_file, line)) {
    
  }
  return 0;
}