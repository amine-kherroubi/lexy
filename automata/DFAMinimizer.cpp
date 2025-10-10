#include "headers/DFAMinimizer.h"
#include "headers/DFA.h"
#include "headers/State.h"
#include <map>
#include <queue>
#include <set>

DFA DFAMinimizer::minimize(DFA &dfa) {
  std::map<Superstate, StateID> superstate_to_state_id_map;
  std::queue<Superstate> superstates_to_process;
  std::set<Superstate> processed_superstates;
  Superstate nonaccepting_superstate, accepting_superstate;
  int next_state_id = 0;

  // Initial partition: accepting vs non-accepting states
  for (State state : dfa.getStates()) {
    if (dfa.isAccepting(state.getID())) {
      accepting_superstate.insert(state.getID());
    } else {
      nonaccepting_superstate.insert(state.getID());
    }
  }

  // Add non-accepting partition
  if (!nonaccepting_superstate.empty()) {
    superstate_to_state_id_map[nonaccepting_superstate] = next_state_id++;
    superstates_to_process.push(nonaccepting_superstate);
  }

  // Add accepting partition
  if (!accepting_superstate.empty()) {
    superstate_to_state_id_map[accepting_superstate] = next_state_id++;
    superstates_to_process.push(accepting_superstate);
  }

  Alphabet alphabet = dfa.getAlphabet();

  // Refine partitions
  while (!superstates_to_process.empty()) {
    Superstate current_superstate = superstates_to_process.front();
    superstates_to_process.pop();

    if (processed_superstates.find(current_superstate) !=
        processed_superstates.end()) {
      continue;
    }
    processed_superstates.insert(current_superstate);

    for (Symbol symbol : alphabet) {
      std::map<StateID, Superstate> transition_map;

      // Group states by their transition target partition
      for (StateID state_id : current_superstate) {
        StateID next_state = dfa.getNextState(state_id, symbol);

        // Skip if no transition exists
        if (next_state == -1) {
          continue;
        }

        // Find which superstate contains next_state
        for (const auto &[target_superstate, target_id] :
             superstate_to_state_id_map) {
          if (target_superstate.find(next_state) != target_superstate.end()) {
            transition_map[target_id].insert(state_id);
            break;
          }
        }
      }

      // Split partition if states transition to different targets
      if (transition_map.size() > 1) {
        superstate_to_state_id_map.erase(current_superstate);

        for (const auto &[target_partition_id, states_in_new_partition] :
             transition_map) {
          if (superstate_to_state_id_map.find(states_in_new_partition) ==
              superstate_to_state_id_map.end()) {
            superstate_to_state_id_map[states_in_new_partition] =
                next_state_id++;
            superstates_to_process.push(states_in_new_partition);
          }
        }
      }
    }
  }

  // Find the new initial state
  StateID old_initial = dfa.getStartStateID();
  StateID new_initial = 0;

  for (const auto &[superstate, new_state_id] : superstate_to_state_id_map) {
    if (superstate.find(old_initial) != superstate.end()) {
      new_initial = new_state_id;
      break;
    }
  }

  // Build minimized DFA
  States minimized_states;
  StateIDs minimized_accepting_ids;

  for (const auto &[superstate, new_state_id] : superstate_to_state_id_map) {
    minimized_states.push_back(State{new_state_id});

    // Check if this partition contains an accepting state
    for (StateID old_id : superstate) {
      if (dfa.isAccepting(old_id)) {
        minimized_accepting_ids.push_back(new_state_id);
        break;
      }
    }
  }

  DFA minimized_dfa{alphabet, minimized_states, minimized_accepting_ids,
                    new_initial};
  minimized_dfa.resizeTransitions(superstate_to_state_id_map.size());

  // Build transitions
  for (const auto &[superstate, from_state_id] : superstate_to_state_id_map) {
    StateID representative = *superstate.begin();

    for (Symbol symbol : alphabet) {
      StateID old_target = dfa.getNextState(representative, symbol);

      // Skip if no transition exists
      if (old_target == -1) {
        continue;
      }

      // Find target partition
      for (const auto &[target_superstate, target_state_id] :
           superstate_to_state_id_map) {
        if (target_superstate.find(old_target) != target_superstate.end()) {
          minimized_dfa.addTransition(from_state_id, symbol, target_state_id);
          break;
        }
      }
    }
  }

  return minimized_dfa;
}