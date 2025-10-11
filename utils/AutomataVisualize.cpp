#include "headers/AutomataVisualizer.h"

using namespace std;

String AutomataVisualizer::escapeLabel(Symbol symbol) {
  if (symbol == '"')
    return "\\\"";
  if (symbol == '\\')
    return "\\\\";
  if (symbol == '\n')
    return "\\n";
  if (symbol == '\t')
    return "\\t";
  return String(1, symbol);
}

void AutomataVisualizer::generateDotFile(const String &dot_content,
                                         const String &filename) {
  ofstream file(filename);
  if (!file.is_open()) {
    cerr << "Error: Could not create file " << filename << endl;
    return;
  }
  file << dot_content;
  file.close();
  cout << "Generated DOT file: " << filename << endl;
}

void AutomataVisualizer::renderDotFile(const String &dot_filename,
                                       const String &image_filename,
                                       const String &output_format) {
  String output_file = image_filename + "." + output_format;
  String command =
      "dot -T" + output_format + " " + dot_filename + " -o " + output_file;

  int result = system(command.c_str());
  if (result == 0) {
    cout << "Rendered visualization: " << output_file << endl;
  } else {
    cerr << "Error: Failed to render with Graphviz. Make sure 'dot' is "
            "installed."
         << endl;
  }
}

void AutomataVisualizer::visualizeNFA(const NFA &nfa, const String &dot_path,
                                      const String &image_path) {
  ostringstream dot;

  dot << "digraph NFA {\n";
  dot << "  rankdir=LR;\n";
  dot << "  node [shape=circle];\n";

  // Mark accepting states with double circles
  StateIDs accepting = nfa.getAcceptingStateIDs();
  if (!accepting.empty()) {
    dot << "  node [shape=doublecircle];";
    for (StateID id : accepting) {
      dot << " " << id;
    }
    dot << ";\n";
  }
  dot << "  node [shape=circle];\n";

  // Invisible start node
  dot << "  start [shape=none, label=\"\"];\n";
  dot << "  start -> " << nfa.getStartStateID() << ";\n";

  // Add transitions
  States states = nfa.getStates();
  for (const State &state : states) {
    StateID from = state.getID();

    // Regular transitions
    Symbols symbols = nfa.getSymbols(from);
    for (Symbol symbol : symbols) {
      StateIDs targets = nfa.getNextStateIDs(from, symbol);
      for (StateID to : targets) {
        dot << "  " << from << " -> " << to << " [label=\""
            << escapeLabel(symbol) << "\"];\n";
      }
    }

    // Epsilon transitions
    StateIDs epsilon_targets = nfa.getEpsilonNextStatesIDs(from);
    for (StateID to : epsilon_targets) {
      dot << "  " << from << " -> " << to << " [label=\"ε\"];\n";
    }
  }

  dot << "}\n";

  String dot_filename = dot_path + ".dot";
  generateDotFile(dot.str(), dot_filename);
  renderDotFile(dot_filename, image_path);
}

void AutomataVisualizer::visualizeDFA(const DFA &dfa, const String &dot_path,
                                      const String &image_path) {
  ostringstream dot;

  dot << "digraph DFA {\n";
  dot << "  rankdir=LR;\n";
  dot << "  node [shape=circle];\n";

  // Mark accepting states with double circles
  StateIDs accepting = dfa.getAcceptingStateIDs();
  if (!accepting.empty()) {
    dot << "  node [shape=doublecircle];";
    for (StateID id : accepting) {
      dot << " " << id;
    }
    dot << ";\n";
  }
  dot << "  node [shape=circle];\n";

  // Invisible start node
  dot << "  start [shape=none, label=\"\"];\n";
  dot << "  start -> " << dfa.getStartStateID() << ";\n";

  // Add transitions
  Alphabet alphabet = dfa.getAlphabet();
  States states = dfa.getStates();
  for (const State &state : states) {
    StateID from = state.getID();

    for (Symbol symbol : alphabet) {
      StateID to = dfa.getNextState(from, symbol);
      if (to != -1) {
        dot << "  " << from << " -> " << to << " [label=\""
            << escapeLabel(symbol) << "\"];\n";
      }
    }
  }

  dot << "}\n";

  String dot_filename = dot_path + ".dot";
  generateDotFile(dot.str(), dot_filename);
  renderDotFile(dot_filename, image_path);
}