#include "../../include/code_generation/code_generator.hpp"
#include <iostream>

void CodeGenerator::generateScanner(const DFA &dfa,
                                    const Vector<String> &token_types,
                                    const String &output_filename) {
  std::ofstream out(output_filename);

  if (!out.is_open()) {
    std::cerr << "Error: Could not create file " << output_filename
              << std::endl;
    return;
  }

  // Build token name to index mapping
  UnorderedMap<String, int> token_type_to_index;
  for (Index i = 0; i < token_types.size(); i++) {
    token_type_to_index[token_types[i]] = static_cast<int>(i);
  }

  // Write header
  out << "#include <string>\n";
  out << "#include <cstring>\n\n";

  // Write transition table
  out << generateTransitionTable(dfa);

  // Write accepting states (now with correct token indices)
  out << generateAcceptingStates(dfa, token_type_to_index);

  // Write token names
  out << generateTokenNames(token_types);

  // Write scanner class
  out << generateScannerClass(dfa);

  out.close();
  std::cout << "Generated scanner: " << output_filename << std::endl;
}

String CodeGenerator::generateTransitionTable(const DFA &dfa) {
  StringStream string_stream;
  Size num_states = dfa.getStates().size();

  string_stream << "static const int TRANSITION_TABLE[" << num_states
                << "][128] = {\n";

  for (const State &state : dfa.getStates()) {
    StateID from = state.getID();
    string_stream << "    {";

    for (int c = 0; c < 128; c++) {
      StateID next = dfa.getNextState(from, static_cast<char>(c));
      string_stream << next;

      if (c < 127)
        string_stream << ", ";
    }

    string_stream << "}";
    if (from < static_cast<int>(num_states) - 1)
      string_stream << ",";
    string_stream << "\n";
  }

  string_stream << "};\n\n";
  return string_stream.str();
}

String CodeGenerator::generateAcceptingStates(
    const DFA &dfa, const UnorderedMap<String, int> &token_type_to_index) {
  StringStream string_stream;
  Size num_states = dfa.getStates().size();

  string_stream << "static const int ACCEPTING_STATES[" << num_states
                << "] = {\n";

  for (const State &state : dfa.getStates()) {
    StateID id = state.getID();

    if (dfa.isAccepting(id)) {
      String token_type = dfa.getTokenType(id);
      auto it = token_type_to_index.find(token_type);
      int token_index = (it != token_type_to_index.end()) ? it->second : 0;
      string_stream << "    " << token_index;
    } else {
      string_stream << "    -1";
    }

    if (id < static_cast<int>(num_states) - 1)
      string_stream << ",";
    string_stream << "\n";
  }

  string_stream << "};\n\n";
  return string_stream.str();
}

String CodeGenerator::generateTokenNames(const Vector<String> &token_types) {
  StringStream string_stream;

  string_stream << "static const char* TOKEN_NAMES[] = {\n";

  for (Index i = 0; i < token_types.size(); i++) {
    string_stream << "    \"" << token_types[i] << "\"";
    if (i < token_types.size() - 1)
      string_stream << ",";
    string_stream << "\n";
  }

  string_stream << "};\n\n";
  return string_stream.str();
}

String CodeGenerator::generateScannerClass(const DFA &dfa) {
  StringStream string_stream;
  StateID start_state = dfa.getStartStateID();

  string_stream << "struct Token {\n";
  string_stream << "    int type;\n";
  string_stream << "    std::string lexeme;\n";
  string_stream << "};\n\n";

  string_stream << "class Scanner {\n";
  string_stream << "private:\n";
  string_stream << "    const char* input;\n";
  string_stream << "    size_t position;\n";
  string_stream << "    size_t length;\n\n";

  string_stream << "public:\n";
  string_stream
      << "    Scanner(const char* input) : input(input), position(0) {\n";
  string_stream << "        length = strlen(input);\n";
  string_stream << "    }\n\n";

  string_stream << "    Token getNextToken() {\n";
  string_stream << "        if (position >= length) {\n";
  string_stream << "            return {-1, \"\"};\n";
  string_stream << "        }\n\n";

  string_stream << "        int current_state = " << start_state << ";\n";
  string_stream << "        size_t start_pos = position;\n";
  string_stream << "        int last_accepting_state = -1;\n";
  string_stream << "        size_t last_accepting_pos = position;\n\n";

  string_stream << "        while (position < length) {\n";
  string_stream << "            char c = input[position];\n";
  string_stream << "            int next_state = "
                   "TRANSITION_TABLE[current_state][(int)c];\n\n";

  string_stream << "            if (next_state == -1) break;\n\n";

  string_stream << "            current_state = next_state;\n";
  string_stream << "            position++;\n\n";

  string_stream << "            if (ACCEPTING_STATES[current_state] != -1) {\n";
  string_stream << "                last_accepting_state = current_state;\n";
  string_stream << "                last_accepting_pos = position;\n";
  string_stream << "            }\n";
  string_stream << "        }\n\n";

  string_stream << "        if (last_accepting_state != -1) {\n";
  string_stream << "            position = last_accepting_pos;\n";
  string_stream
      << "            std::string lexeme(input + start_pos, last_accepting_pos "
         "- start_pos);\n";
  string_stream << "            int token_type = "
                   "ACCEPTING_STATES[last_accepting_state];\n";
  string_stream << "            return {token_type, lexeme};\n";
  string_stream << "        }\n\n";

  string_stream << "        position = start_pos + 1;\n";
  string_stream << "        return {-2, std::string(1, input[start_pos])};\n";
  string_stream << "    }\n";
  string_stream << "};\n\n";

  return string_stream.str();
}