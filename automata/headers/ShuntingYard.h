#include <string>
#include <vector>

class ShuntingYard {
public:
  static std::vector<char> convertToRPN(const std::string &regex);

private:
  static bool isOperator(char c);
  static bool isOperand(char c);
  static int precedence(char op);
  static bool isLeftAssociative(char op);
};
