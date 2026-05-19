#include "nfa_determinizer.hpp"
#include <climits>

Closure NFADeterminizer::epsilonClosure(const NFA &nfa, StateID state_id) {
  Closure closure;
  Queue<StateID> state_ids_to_process;

  closure.insert(state_id);
  state_ids_to_process.push(state_id);

  while (!state_ids_to_process.empty()) {
    StateID current_state_id = state_ids_to_process.front();
    state_ids_to_process.pop();

    const StateIDs &epsilon_reachable_states =
        nfa.getEpsilonNextStatesIDs(current_state_id);

    for (StateID next_state_id : epsilon_reachable_states) {
      if (closure.find(next_state_id) == closure.end()) {
        closure.insert(next_state_id);
        state_ids_to_process.push(next_state_id);
      }
    }
  }

  return closure;
}

Closure NFADeterminizer::epsilonClosure(const NFA &nfa,
                                        const Superstate &superstate) {
  Closure closure;

  if (superstate.empty()) {
    return closure;
  }

  for (StateID state_id : superstate) {
    Closure subclosure = epsilonClosure(nfa, state_id);
    closure.insert(subclosure.begin(), subclosure.end());
  }

  return closure;
}

Superstate NFADeterminizer::move(const NFA &nfa, const Superstate &superstate,
                                 Symbol symbol) {
  Superstate result;

  for (StateID state_id : superstate) {
    const StateIDs &next_states = nfa.getNextStateIDs(state_id, symbol);
    result.insert(next_states.begin(), next_states.end());
  }

  return epsilonClosure(nfa, result);
}

bool NFADeterminizer::containsAcceptingState(const NFA &nfa,
                                             const Superstate &superstate) {
  for (StateID state_id : superstate) {
    if (nfa.isAccepting(state_id)) {
      return true;
    }
  }
  return false;
}

// Picks the highest-priority (lowest declaration index) token type among all
// accepting NFA states in the superstate. This correctly implements the
// "first declaration wins" rule: e.g. RETURN beats IDENTIFIER when both
// match, because RETURN was declared earlier by the user.
//
// Previously this iterated a Set<StateID> and returned the first accepting
// state found — but set iteration order is by numeric ID, which reflects
// internal NFA construction order, not user declaration order.
String NFADeterminizer::resolveTokenType(
    const NFA &nfa, const Superstate &superstate,
    const UnorderedMap<String, int> &token_priority) {

  String best_token;
  int best_priority = INT_MAX;

  for (StateID id : superstate) {
    if (!nfa.isAccepting(id))
      continue;

    String token_type = nfa.getTokenType(id);
    if (token_type.empty())
      continue;

    auto it = token_priority.find(token_type);
    // If not found in priority map, treat as lowest priority (INT_MAX - 1)
    // so it can still win over an empty token type.
    int priority = (it != token_priority.end()) ? it->second : (INT_MAX - 1);

    if (priority < best_priority) {
      best_priority = priority;
      best_token = token_type;
    }
  }

  return best_token;
}

DFA NFADeterminizer::determinize(
    const NFA &nfa, const UnorderedMap<String, int> &token_priority) {
  Superstate start_superstate = epsilonClosure(nfa, nfa.getStartStateID());
  Map<Superstate, StateID> superstate_to_state_id_map;
  Queue<Superstate> superstates_to_process;

  States dfa_states;
  UnorderedMap<StateID, String> dfa_accepting_map;

  // Add initial state
  superstate_to_state_id_map[start_superstate] = 0;
  dfa_states.push_back(State(0));
  superstates_to_process.push(start_superstate);

  if (containsAcceptingState(nfa, start_superstate)) {
    dfa_accepting_map[0] =
        resolveTokenType(nfa, start_superstate, token_priority);
  }

  const Alphabet alphabet = nfa.getAlphabet();

  DFA dfa(alphabet, dfa_states, dfa_accepting_map, 0);
  dfa.resizeTransitions(1);

  while (!superstates_to_process.empty()) {
    Superstate current_superstate = superstates_to_process.front();
    superstates_to_process.pop();

    StateID current_dfa_state = superstate_to_state_id_map[current_superstate];

    for (Symbol symbol : alphabet) {
      Superstate next_superstate = move(nfa, current_superstate, symbol);

      if (next_superstate.empty()) {
        continue;
      }

      // Check if this superstate is new
      if (superstate_to_state_id_map.find(next_superstate) ==
          superstate_to_state_id_map.end()) {
        StateID new_id = dfa_states.size();
        superstate_to_state_id_map[next_superstate] = new_id;
        dfa_states.push_back(State{new_id});
        superstates_to_process.push(next_superstate);

        if (containsAcceptingState(nfa, next_superstate)) {
          dfa_accepting_map[new_id] =
              resolveTokenType(nfa, next_superstate, token_priority);
        }

        dfa.resizeTransitions(dfa_states.size());
      }

      StateID next_dfa_state = superstate_to_state_id_map[next_superstate];
      dfa.addTransition(current_dfa_state, symbol, next_dfa_state);
    }
  }

  // Update the DFA with the final states and accepting states
  dfa.getStates() = dfa_states;
  dfa.getAcceptingStateIDsToTokenTypes() = dfa_accepting_map;

  return dfa;
}
