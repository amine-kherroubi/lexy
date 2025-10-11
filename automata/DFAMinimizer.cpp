#include "headers/DFAMinimizer.h"
#include "headers/DFA.h"
#include "headers/State.h"
#include <map>
#include <queue>
#include <set>
#include <vector>

DFA DFAMinimizer::minimize(const DFA &dfa) {
  // Step 0: Find reachable states
  std::set<StateID> reachable;
  std::queue<StateID> to_visit;

  StateID start = dfa.getStartStateID();
  reachable.insert(start);
  to_visit.push(start);

  Alphabet alphabet = dfa.getAlphabet();

  while (!to_visit.empty()) {
    StateID current = to_visit.front();
    to_visit.pop();

    for (Symbol symbol : alphabet) {
      StateID next = dfa.getNextState(current, symbol);
      if (next != -1 && reachable.find(next) == reachable.end()) {
        reachable.insert(next);
        to_visit.push(next);
      }
    }
  }

  // Step 1: Create initial partitions (accepting vs non-accepting)
  // Only include reachable states
  std::vector<Superstate> partitions;
  Superstate nonaccepting_partition;
  Superstate accepting_partition;

  for (const State &state : dfa.getStates()) {
    StateID id = state.getID();
    // Only consider reachable states
    if (reachable.find(id) == reachable.end()) {
      continue;
    }

    if (dfa.isAccepting(id)) {
      accepting_partition.insert(id);
    } else {
      nonaccepting_partition.insert(id);
    }
  }

  if (!nonaccepting_partition.empty()) {
    partitions.push_back(nonaccepting_partition);
  }
  if (!accepting_partition.empty()) {
    partitions.push_back(accepting_partition);
  }

  // Step 2: Refine partitions until no more splits occur
  bool changed = true;
  while (changed) {
    changed = false;
    std::vector<Superstate> new_partitions;

    for (const Superstate &partition : partitions) {
      // For each partition, try to split it
      std::map<std::vector<int>, Superstate> signature_to_states;

      for (StateID state_id : partition) {
        // Create signature: for each symbol, which partition does it go to?
        std::vector<int> signature;

        for (Symbol symbol : alphabet) {
          StateID next_state = dfa.getNextState(state_id, symbol);

          // Find which partition contains next_state
          int target_partition_index = -1;
          if (next_state != -1) {
            for (size_t i = 0; i < partitions.size(); i++) {
              if (partitions[i].find(next_state) != partitions[i].end()) {
                target_partition_index = static_cast<int>(i);
                break;
              }
            }
          }

          signature.push_back(target_partition_index);
        }

        // Group states with same signature
        signature_to_states[signature].insert(state_id);
      }

      // If we have multiple groups, the partition was split
      if (signature_to_states.size() > 1) {
        changed = true;
        for (const auto &[signature, states] : signature_to_states) {
          new_partitions.push_back(states);
        }
      } else {
        // No split, keep the partition as is
        new_partitions.push_back(partition);
      }
    }

    partitions = new_partitions;
  }

  // Step 3: Build the minimized DFA
  // Create mapping from old state ID to new partition ID
  std::map<StateID, StateID> old_to_new_state;
  for (size_t i = 0; i < partitions.size(); i++) {
    for (StateID old_id : partitions[i]) {
      old_to_new_state[old_id] = static_cast<StateID>(i);
    }
  }

  // Find new initial state
  StateID old_initial = dfa.getStartStateID();
  StateID new_initial = old_to_new_state[old_initial];

  // Build new states and accepting states
  States minimized_states;
  StateIDs minimized_accepting_ids;

  for (size_t i = 0; i < partitions.size(); i++) {
    minimized_states.push_back(State{static_cast<int>(i)});

    // Check if this partition contains an accepting state
    bool is_accepting = false;
    for (StateID old_id : partitions[i]) {
      if (dfa.isAccepting(old_id)) {
        is_accepting = true;
        break;
      }
    }

    if (is_accepting) {
      minimized_accepting_ids.push_back(static_cast<StateID>(i));
    }
  }

  // Build minimized DFA
  DFA minimized_dfa{alphabet, minimized_states, minimized_accepting_ids,
                    new_initial};
  minimized_dfa.resizeTransitions(partitions.size());

  // Build transitions using a representative from each partition
  for (size_t i = 0; i < partitions.size(); i++) {
    StateID representative = *partitions[i].begin();

    for (Symbol symbol : alphabet) {
      StateID old_target = dfa.getNextState(representative, symbol);

      if (old_target != -1) {
        StateID new_target = old_to_new_state[old_target];
        minimized_dfa.addTransition(static_cast<StateID>(i), symbol,
                                    new_target);
      }
    }
  }

  return minimized_dfa;
}