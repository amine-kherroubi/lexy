#include <string>
#include <vector>

struct Alphabet {
private:
  std::string name;
  std::vector<char> symbols;

public:
  Alphabet(const std::string &name, const std::vector<char> &symbols)
      : name{name}, symbols{symbols} {}
};