#include "../automata/state.hpp"
#include <cstddef>
#include <fstream>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

template <typename T> using Vector = std::vector<T>;
template <typename T> using Set = std::set<T>;
template <typename T> using Queue = std::queue<T>;
template <typename T> using Stack = std::stack<T>;
template <typename Key, typename Value> using Map = std::map<Key, Value>;
template <typename Key, typename Value>
using UnorderedMap = std::unordered_map<Key, Value>;
template <typename T> using Pointer = std::unique_ptr<T>;
template <typename T1, typename T2> using Pair = std::pair<T1, T2>;
using File = std::basic_ifstream<char>;
using StringStream = std::ostringstream;

using String = std::string;

using StateID = int;
using StateIDs = Vector<StateID>;
using States = Vector<State>;

using Symbol = char;
using Symbols = Vector<Symbol>;
using Alphabet = Set<Symbol>;

using Superstate = Set<StateID>;
using Closure = Set<StateID>;

using Lexeme = String;
using Index = std::size_t;
using Size = std::size_t;