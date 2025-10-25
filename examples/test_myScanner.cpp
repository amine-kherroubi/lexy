#include "../generated/scanners/myScanner.cpp"
#include <iostream>

int main() {
  std::cout << "=== Testing myScanner ===" << std::endl;
  std::cout << std::endl;

  // Test cases
  const char *test_inputs[] = {
      "hello", "world123", "_identifier", "123",
      "0",     "999",      "abc_123_def", "12hello",
      "0x123", "",         "  spaces  ",  "mix123abc456"};

  for (const char *input : test_inputs) {
    std::cout << "Input: \"" << input << "\"" << std::endl;

    Scanner scanner(input);
    Token token;

    do {
      token = scanner.getNextToken();

      if (token.type == -1) {
        std::cout << "  EOF" << std::endl;
      } else if (token.type == -2) {
        std::cout << "  ERROR: '" << token.lexeme << "'" << std::endl;
      } else {
        std::cout << "  " << TOKEN_NAMES[token.type] << ": \"" << token.lexeme
                  << "\"" << std::endl;
      }
    } while (token.type != -1);

    std::cout << std::endl;
  }

  std::cout << "=== All tests complete ===" << std::endl;
  return 0;
}