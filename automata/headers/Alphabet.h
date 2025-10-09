#include <string>
#include <vector>
using Symbol = char;
using Symbols = std::vector<Symbol>;

struct Alphabet {
private:
  std::string name;
  Symbols symbols;

public:
  Alphabet(const std::string &name, const Symbols &symbols)
      : name{name}, symbols{symbols} {}
};