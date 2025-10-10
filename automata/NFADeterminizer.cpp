#include "headers/NFADeterminizer.h"
#include <map>
#include <queue>

DFA NFADeterminizer::determinize(const NFA &nfa) {
  std::set<Symbol> alphabet = collectAlphabet(nfa);
  std::set<StateID> start_set = epsilonClosure(nfa, nfa.getStartStateID());
  std::map<std::set<StateID>, StateID> state_map;
  std::queue<std::set<StateID>> unmarked_states;

  States dfa_states;
  StateIDs accepting_state_ids;

  state_map[start_set] = 0;
  dfa_states.push_back(State{0});
  unmarked_states.push(start_set);

  // Check if start state is accepting
  if (containsAcceptingState(nfa, start_set)) {
    accepting_state_ids.push_back(0);
  }

  // Create DFA and initialize transitions
  DFA dfa{dfa_states, accepting_state_ids, 0};

  while (!unmarked_states.empty()) {
    std::set<StateID> current_set = unmarked_states.front();
    unmarked_states.pop();

    StateID current_dfa_state = state_map[current_set];

    // For each symbol in alphabet
    for (Symbol symbol : alphabet) {
      // Compute where this set of states goes on this symbol
      std::set<StateID> next_set = move(nfa, current_set, symbol);

      // Skip if empty (no transition)
      if (next_set.empty()) {
        continue;
      }

      // Check if this state set already exists
      if (state_map.find(next_set) == state_map.end()) {
        // New state: add it
        StateID new_id = dfa_states.size();
        state_map[next_set] = new_id;
        dfa_states.push_back(State{new_id});
        unmarked_states.push(next_set);

        // Check if accepting
        if (containsAcceptingState(nfa, next_set)) {
          accepting_state_ids.push_back(new_id);
        }
      }

      // Add transition in DFA
      StateID next_dfa_state = state_map[next_set];
      dfa.addTransition(current_dfa_state, symbol, next_dfa_state);
    }
  }

  // Update DFA with final states and accepting states
  dfa.getStates() = dfa_states;
  dfa.getAcceptingStateIDs() = accepting_state_ids;
  dfa.resizeTransitions(dfa_states.size());

  return dfa;
}

std::set<Symbol> NFADeterminizer::collectAlphabet(const NFA &nfa) {
  std::set<Symbol> alphabet;

  // Collect all symbols from all states
  for (const State &state : nfa.getStates()) {
    Symbols symbols = nfa.getSymbols(state.getID());
    for (Symbol symbol : symbols) {
      alphabet.insert(symbol);
    }
  }

  return alphabet;
}

std::set<StateID> NFADeterminizer::move(const NFA &nfa,
                                        const std::set<StateID> &states,
                                        Symbol symbol) {
  std::set<StateID> result;

  for (StateID state : states) {
    const StateIDs &next_states = nfa.getNextStates(state, symbol);

    for (StateID next : next_states) {
      result.insert(next);
    }
  }

  result = epsilonClosure(nfa, result);

  return result;
}

bool NFADeterminizer::containsAcceptingState(const NFA &nfa,
                                             const std::set<StateID> &states) {
  for (StateID state : states) {
    if (nfa.isAccepting(state)) {
      return true;
    }
  }
  return false;
}