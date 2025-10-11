#include "../automata/state.hpp"
#include <cstddef>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

template <typename Tp> using Vector = std::vector<Tp>;
template <typename Tp> using Set = std::set<Tp>;
template <typename Tp> using Queue = std::queue<Tp>;
template <typename Tp> using Stack = std::stack<Tp>;
template <typename Key, typename Value> using Map = std::map<Key, Value>;
template <typename Key, typename Value>
using UnorderedMap = std::unordered_map<Key, Value>;
template <typename Tp> using Pointer = std::unique_ptr<Tp>;

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