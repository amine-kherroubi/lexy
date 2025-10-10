#include "headers/NFADeterminizer.h"
#include <map>
#include <queue>

Closure NFADeterminizer::epsilonClosure(const NFA &nfa, StateID state) {
  Closure closure;
  std::queue<StateID> states_to_process;
  closure.insert(state);
  states_to_process.push(state);
  while (!states_to_process.empty()) {
    StateID current = states_to_process.front();
    states_to_process.pop();
    const StateIDs &epsilon_reachable_states =
        nfa.getEpsilonNextStates(current);
    for (StateID next_state : epsilon_reachable_states) {
      if (closure.find(next_state) == closure.end()) {
        closure.insert(next_state);
        states_to_process.push(next_state);
      }
    }
  }
  return closure;
}

Closure NFADeterminizer::epsilonClosure(const NFA &nfa,
                                        const Superstate &superstate) {
  Closure closure;
  for (StateID state : superstate) {
    Closure subclosure = epsilonClosure(nfa, state);
    closure.insert(subclosure.begin(), subclosure.end());
  }
  return closure;
}

Alphabet NFADeterminizer::collectAlphabet(const NFA &nfa) {
  Alphabet alphabet;
  for (const State &state : nfa.getStates()) {
    Symbols symbols = nfa.getSymbols(state.getID());
    alphabet.insert(symbols.begin(), symbols.end());
  }
  return alphabet;
}

/**
 * @brief Computes the move operation: states reachable from a set via a symbol
 *
 * The move operation answers: "If I'm in any of these states and I read this
 * symbol, what states could I end up in (including epsilon transitions)?"
 *
 * Process:
 * 1. For each state in the input set
 * 2. Find all states directly reachable via the given symbol
 * 3. Compute epsilon closure of those states
 *
 * @param nfa The NFA to operate on
 * @param states Set of current states
 * @param symbol The input symbol
 * @return Set of states reachable via symbol (with epsilon closure)
 */
Superstate NFADeterminizer::move(const NFA &nfa, const Superstate &states,
                                 Symbol symbol) {
  Superstate result;

  // For each state in our current set
  for (StateID state : states) {
    // Get all states reachable via the symbol
    const StateIDs &next_states = nfa.getNextStates(state, symbol);
    result.insert(next_states.begin(), next_states.end());
  }

  // Apply epsilon closure to include epsilon-reachable states
  return epsilonClosure(nfa, result);
}

/**
 * @brief Checks if a set of NFA states contains at least one accepting state
 *
 * In DFA construction, a DFA state (which represents a set of NFA states)
 * is accepting if ANY of the NFA states in that set is accepting.
 *
 * @param nfa The NFA to check against
 * @param states Set of NFA state IDs
 * @return true if at least one state is accepting, false otherwise
 */
bool NFADeterminizer::containsAcceptingState(const NFA &nfa,
                                             const Superstate &states) {
  for (StateID state : states) {
    if (nfa.isAccepting(state)) {
      return true;
    }
  }
  return false;
}

/**
 * @brief Converts an NFA to a DFA using subset construction
 *
 * Algorithm (Subset Construction):
 * 1. Start with epsilon closure of NFA start state as DFA start state
 * 2. For each unmarked DFA state (set of NFA states):
 *    a. Mark it as processed
 *    b. For each symbol in the alphabet:
 *       - Compute move(current_set, symbol) = next_set
 *       - If next_set is new, add it as a new DFA state
 *       - Add transition: current_dfa_state --symbol--> next_dfa_state
 * 3. A DFA state is accepting if it contains any NFA accepting state
 *
 * Time Complexity: O(2^n * |alphabet|) in worst case, where n = number of NFA
 * states Space Complexity: O(2^n) for storing DFA states
 *
 * @param nfa The NFA to convert
 * @return Equivalent DFA
 */
DFA NFADeterminizer::determinize(const NFA &nfa) {
  // Step 1: Collect the alphabet (all symbols used in NFA)
  std::set<Symbol> alphabet = collectAlphabet(nfa);

  // Step 2: Compute starting DFA state (epsilon closure of NFA start state)
  Superstate start_set = epsilonClosure(nfa, nfa.getStartStateID());

  // Maps: NFA state set -> DFA state ID
  // This tracks which DFA state corresponds to which set of NFA states
  std::map<Superstate, StateID> state_map;

  // Queue of DFA states (as NFA state sets) that need processing
  std::queue<Superstate> unmarked_states;

  // DFA construction data
  States dfa_states;
  StateIDs accepting_state_ids;

  // Initialize DFA with start state (ID = 0)
  state_map[start_set] = 0;
  dfa_states.push_back(State{0});
  unmarked_states.push(start_set);

  // Check if start state should be accepting
  if (containsAcceptingState(nfa, start_set)) {
    accepting_state_ids.push_back(0);
  }

  // Step 3: Create DFA object and prepare transition table
  DFA dfa{dfa_states, accepting_state_ids, 0};
  dfa.resizeTransitions(1); // Start with one state

  // Step 4: Process all unmarked states (Subset Construction main loop)
  while (!unmarked_states.empty()) {
    // Get next unmarked state to process
    Superstate current_set = unmarked_states.front();
    unmarked_states.pop();

    StateID current_dfa_state = state_map[current_set];

    // For each symbol in the alphabet, compute transitions
    for (Symbol symbol : alphabet) {
      // Compute: where do we go from current_set on symbol?
      Superstate next_set = move(nfa, current_set, symbol);

      // Skip if no transition exists (dead end)
      if (next_set.empty()) {
        continue;
      }

      // Check if this state set is new (not yet in DFA)
      if (state_map.find(next_set) == state_map.end()) {
        // Create new DFA state
        StateID new_id = dfa_states.size();
        state_map[next_set] = new_id;
        dfa_states.push_back(State{new_id});
        unmarked_states.push(next_set);

        // Check if this new state should be accepting
        if (containsAcceptingState(nfa, next_set)) {
          accepting_state_ids.push_back(new_id);
        }

        // Expand transition table for new state
        dfa.resizeTransitions(dfa_states.size());
      }

      // Add transition: current_dfa_state --symbol--> next_dfa_state
      StateID next_dfa_state = state_map[next_set];
      dfa.addTransition(current_dfa_state, symbol, next_dfa_state);
    }
  }

  // Step 5: Update DFA with final states and accepting states
  dfa.getStates() = dfa_states;
  dfa.getAcceptingStateIDs() = accepting_state_ids;

  return dfa;
}