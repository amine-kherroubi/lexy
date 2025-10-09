#include "NFA.h"
#include <string>
#include <vector>

class RegexToNFA {
public:
  static NFA convert(const std::string &regex);
  static void resetIds();

private:
  static StateID nextId;

  static NFA buildForSymbol(Symbol c);
  static NFA concatenate(const NFA &left, const NFA &right);
  static NFA alternate(const NFA &left, const NFA &right);
  static NFA kleeneStar(const NFA &nfa);
  static std::vector<char> shuntingYard(const std::string &regex);
  static bool isOperator(char c);
  static bool isOperand(char c);
  static int precedence(char op);
  static bool isLeftAssociative(char op);
};
