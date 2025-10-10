#include "../automata/headers/State.h"
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

using String = std::string;

using StateID = int;
using StateIDs = std::vector<StateID>;
using States = std::vector<State>;

using Symbol = char;
using Symbols = std::vector<Symbol>;
using Alphabet = std::set<Symbol>;

using DeterministicTransitions =
    std::vector<std::unordered_map<Symbol, StateID>>;

using NondeterministicTransitions =
    std::vector<std::unordered_map<Symbol, StateIDs>>;
using EpsilonTransitions = std::vector<StateIDs>;

using Superstate = std::set<StateID>;
using Closure = std::set<StateID>;