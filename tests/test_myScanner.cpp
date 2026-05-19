#include "../build/scanners/myScanner.cpp"
#include <iostream>
#include <string>
#include <vector>

struct TestCase {
  std::string input;
  std::vector<std::string> expected_tokens;
  bool expect_error;
};

int main() {
  std::cout << "=== Testing myScanner ===" << std::endl;
  std::cout << std::endl;

  std::vector<TestCase> test_cases = {
      {"hello", {"IDENTIFIER"}, false},
      {"world123", {"IDENTIFIER"}, false},
      {"_identifier", {"IDENTIFIER"}, false},
      {"123", {"INTEGER"}, false},
      {"0", {"INTEGER"}, false},
      {"999", {"INTEGER"}, false},
      {"abc_123_def", {"IDENTIFIER"}, false},
      {"12hello", {"INTEGER", "IDENTIFIER"}, false},
      {"0x123",
       {"INTEGER", "IDENTIFIER"},
       false}, // Expected behavior based on grammar
      {"", {}, false},
      {"  spaces  ", {"IDENTIFIER"}, true}, // Should have errors due to spaces
      {"mix123abc456", {"IDENTIFIER"}, false}};

  int passed = 0;
  for (const auto &tc : test_cases) {
    std::cout << "Testing Input: \"" << tc.input << "\"" << std::endl;

    Scanner scanner(tc.input.c_str());
    std::vector<std::string> actual_tokens;
    bool error_found = false;

    Token token;
    do {
      token = scanner.getNextToken();
      if (token.type == -1) {
        // EOF
      } else if (token.type == -2) {
        error_found = true;
        std::cout << "  [!] ERROR: '" << token.lexeme << "'" << std::endl;
      } else {
        actual_tokens.push_back(TOKEN_NAMES[token.type]);
        std::cout << "  " << TOKEN_NAMES[token.type] << ": \"" << token.lexeme
                  << "\"" << std::endl;
      }
    } while (token.type != -1);

    if (actual_tokens == tc.expected_tokens && error_found == tc.expect_error) {
      std::cout << "  Status: PASSED" << std::endl;
      passed++;
    } else {
      std::cout << "  Status: FAILED" << std::endl;
    }
    std::cout << std::endl;
  }

  std::cout << "=== Test Summary: " << passed << "/" << test_cases.size()
            << " passed ===" << std::endl;
  return (passed == (int)test_cases.size()) ? 0 : 1;
}
