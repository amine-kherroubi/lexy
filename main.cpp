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
#include "src/visualization/automata_visualizer.hpp"
#include <filesystem>
#include <getopt.h>
#include <iostream>

namespace fs = std::filesystem;
using namespace std;

void printUsage(const char *progName) {
  cout << "Usage: " << progName << " <input_file.lexy> [options]\n"
       << "Options:\n"
       << "  -o <dir>     Output directory for generated files (default: "
          "./output)\n"
       << "  -g           Enable automata graph generation\n"
       << "  -h           Show this help message\n";
}

int main(int argc, char *argv[]) {
  String input_filename;
  String output_dir = "output";
  bool generate_graphs = false;

  int opt;
  while ((opt = getopt(argc, argv, "o:gh")) != -1) {
    switch (opt) {
    case 'o':
      output_dir = optarg;
      break;
    case 'g':
      generate_graphs = true;
      break;
    case 'h':
      printUsage(argv[0]);
      return 0;
    default:
      printUsage(argv[0]);
      return -1;
    }
  }

  if (optind < argc) {
    input_filename = argv[optind];
  } else {
    cerr << "Error: Missing input file.\n";
    printUsage(argv[0]);
    return -1;
  }

  if (!hasLexyExtension(input_filename)) {
    cerr << "Error: Input file must have .lexy extension.\n";
    return -1;
  }

  File spec_file(input_filename);
  if (!spec_file.is_open()) {
    cerr << "Error: Failed to open '" << input_filename << "' for reading.\n";
    return -1;
  }

  String specifications((istreambuf_iterator<char>(spec_file)),
                        istreambuf_iterator<char>());
  spec_file.close();

  UserSpecScanner user_spec_scanner(specifications);
  UserSpecParser user_spec_parser(user_spec_scanner);
  Map<String, String> user_token_types = user_spec_parser.parse();

  Vector<String> token_types;
  Vector<NFA> nfas;

  for (const auto &[token_type, regex] : user_token_types) {
    cout << "Processing token: " << token_type << endl;
    RegexScanner regex_scanner(regex);
    RegexParser regex_parser(regex_scanner);
    Pointer<RegexASTNode> regex_ast = regex_parser.parse();
    NFA nfa = RegexASTToNFA::convert(regex_ast, token_type);
    nfas.push_back(nfa);
    token_types.push_back(token_type);
  }

  NFA merged_nfa = ThompsonConstruction::mergeAll(nfas);
  DFA dfa = NFADeterminizer::determinize(merged_nfa);
  DFA minimized = DFAMinimizer::minimize(dfa);

  // Initialize output directory structure
  fs::path out_path(output_dir);
  fs::path scanner_path = out_path / "scanners";
  fs::create_directories(scanner_path);

  if (generate_graphs) {
    fs::path graphviz_path = out_path / "graphviz";
    fs::path images_path = out_path / "images";
    fs::create_directories(graphviz_path);
    fs::create_directories(images_path);

#ifdef HAVE_GRAPHVIZ
    AutomataVisualizer::visualizeNFA(merged_nfa,
                                     (graphviz_path / "nfa").string(),
                                     (images_path / "nfa").string());
    AutomataVisualizer::visualizeDFA(dfa, (graphviz_path / "dfa").string(),
                                     (images_path / "dfa").string());
    AutomataVisualizer::visualizeDFA(minimized,
                                     (graphviz_path / "dfa_minimized").string(),
                                     (images_path / "dfa_minimized").string());

    fs::remove_all(graphviz_path); // Remove temporary graphviz directory
#else
    cout << "Warning: Graph generation requested but Graphviz (dot) not "
            "found.\n";
#endif
  }

  String base_name = getBaseName(input_filename);
  String output_filename = (scanner_path / (base_name + ".cpp")).string();

  CodeGenerator::generateScanner(minimized, token_types, output_filename);

  cout << "\nScanner generated successfully in: " << output_filename << endl;
  return 0;
}
