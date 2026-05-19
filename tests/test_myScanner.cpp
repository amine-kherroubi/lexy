#include "../build/scanners/myScanner.cpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

int main() {
  std::ifstream file("tests/sample_program.rs");
  if (!file.is_open()) {
    std::cerr << "Error: Could not open tests/sample_program.rs" << std::endl;
    return 1;
  }

  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string input = buffer.str();
  file.close();

  std::cout << "=== Testing Generated Scanner with Input File ===" << std::endl;
  std::cout << "Input content:\n" << input << "\n" << std::endl;

  Scanner scanner(input.c_str());
  Token token;

  do {
    token = scanner.getNextToken();
    if (token.type == -1) {
      // End of file reached
    } else if (token.type == -2) {
      std::cout << "Error: Invalid token '" << token.lexeme << "'" << std::endl;
    } else {
      std::cout << "Token: " << TOKEN_NAMES[token.type] << " ('" << token.lexeme
                << "')" << std::endl;
    }
  } while (token.type != -1);

  std::cout << "\nScanner test completed." << std::endl;
  return 0;
}
