/**
 * @file NFADeterminizer.cpp
 * @brief Converts Non-deterministic Finite Automata (NFA) to Deterministic
 * Finite Automata (DFA)
 *
 * This implementation uses the subset construction algorithm (powerset
 * construction). Each state in the resulting DFA represents a set of states
 * from the original NFA.
 *
 * Key Concepts:
 * - Epsilon Closure: All states reachable from a state using only epsilon
 * transitions
 * - Move: All states reachable from a set of states using a specific symbol
 * - Subset Construction: Building DFA states from sets of NFA states
 */

#include "headers/NFADeterminizer.h"
#include <map>
#include <queue>

/**
 * @brief Computes epsilon closure for a single state
 *
 * The epsilon closure of a state is the set of all states reachable from that
 * state using only epsilon (empty string) transitions, including the state
 * itself.
 *
 * Algorithm:
 * 1. Start with the input state in the closure
 * 2. Use a queue to process states
 * 3. For each state, add all epsilon-reachable states
 * 4. Continue until no new states are found
 *
 * @param nfa The NFA to operate on
 * @param state The starting state ID
 * @return Set of all states in the epsilon closure
 */
std::set<StateID> NFADeterminizer::epsilonClosure(const NFA &nfa,
                                                  StateID state) {
  std::set<StateID> closure;
  std::queue<StateID> to_process;

  // Initialize: add the starting state
  closure.insert(state);
  to_process.push(state);

  // Process all reachable states via epsilon transitions
  while (!to_process.empty()) {
    StateID current = to_process.front();
    to_process.pop();

    // Get all states reachable via epsilon from current state
    const StateIDs &epsilon_next = nfa.getEpsilonNextStates(current);

    for (StateID next_state : epsilon_next) {
      // Only process if we haven't seen this state before
      if (closure.find(next_state) == closure.end()) {
        closure.insert(next_state);
        to_process.push(next_state);
      }
    }
  }

  return closure;
}

/**
 * @brief Computes epsilon closure for a set of states
 *
 * This is the union of epsilon closures for each individual state in the set.
 *
 * @param nfa The NFA to operate on
 * @param states Set of starting states
 * @return Set of all states in the combined epsilon closure
 */
std::set<StateID>
NFADeterminizer::epsilonClosure(const NFA &nfa,
                                const std::set<StateID> &states) {
  std::set<StateID> result;

  // Compute epsilon closure for each state and combine results
  for (StateID state : states) {
    std::set<StateID> closure = epsilonClosure(nfa, state);
    result.insert(closure.begin(), closure.end());
  }

  return result;
}

/**
 * @brief Collects all symbols used in the NFA (the alphabet)
 *
 * Scans all transitions in the NFA to build the complete alphabet.
 * Epsilon transitions are not included as they are not part of the input
 * alphabet.
 *
 * @param nfa The NFA to analyze
 * @return Set of all symbols used in transitions
 */
std::set<Symbol> NFADeterminizer::collectAlphabet(const NFA &nfa) {
  std::set<Symbol> alphabet;

  // Iterate through all states and collect their transition symbols
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
std::set<StateID> NFADeterminizer::move(const NFA &nfa,
                                        const std::set<StateID> &states,
                                        Symbol symbol) {
  std::set<StateID> result;

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
                                             const std::set<StateID> &states) {
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
  std::set<StateID> start_set = epsilonClosure(nfa, nfa.getStartStateID());

  // Maps: NFA state set -> DFA state ID
  // This tracks which DFA state corresponds to which set of NFA states
  std::map<std::set<StateID>, StateID> state_map;

  // Queue of DFA states (as NFA state sets) that need processing
  std::queue<std::set<StateID>> unmarked_states;

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
    std::set<StateID> current_set = unmarked_states.front();
    unmarked_states.pop();

    StateID current_dfa_state = state_map[current_set];

    // For each symbol in the alphabet, compute transitions
    for (Symbol symbol : alphabet) {
      // Compute: where do we go from current_set on symbol?
      std::set<StateID> next_set = move(nfa, current_set, symbol);

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