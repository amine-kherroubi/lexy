#include "headers/DFAMinimizer.h"
#include "headers/DFA.h"
#include "headers/State.h"
#include <map>
#include <queue>

DFA DFAMinimizer::minimize(DFA &dfa) {
  std::map<Superstate, StateID> superstate_to_state_id_map;
  std::queue<Superstate> superstates_to_process;

  States minimized_dfa_states;
  StateIDs minimized_dfa_accepting_state_ids;
  int next_state_id = 0;

  Superstate nonaccepting_superstate, accepting_superstate;

  for (State state : dfa.getStates()) {
    if (dfa.isAccepting(state.getID())) {
      accepting_superstate.insert(state.getID());

    } else {
      nonaccepting_superstate.insert(state.getID());
    }
  }

  if (!nonaccepting_superstate.empty()) {
    superstate_to_state_id_map[nonaccepting_superstate] = next_state_id;
    minimized_dfa_states.push_back(State{next_state_id++});
    superstates_to_process.push(nonaccepting_superstate);
  }

  if (!accepting_superstate.empty()) {
    superstate_to_state_id_map[accepting_superstate] = next_state_id;
    minimized_dfa_states.push_back(State{next_state_id});
    minimized_dfa_accepting_state_ids.push_back(next_state_id++);
    superstates_to_process.push(accepting_superstate);
  }

  Alphabet alphabet = dfa.getAlphabet();

  DFA minimized_dfa{alphabet, minimized_dfa_states,
                    minimized_dfa_accepting_state_ids, 0};

  while (!superstates_to_process.empty()) {
    Superstate current_superstate = superstates_to_process.front();
    superstates_to_process.pop();

    for (Symbol symbol : alphabet) {
      Superstate inside_superstate;
      Superstate outside_superstate;

      for (State state : dfa.getStates()) {
        StateID next_state_id = dfa.getNextState(state.getID(), symbol);
        if (current_superstate.find(next_state_id) ==
            current_superstate.end()) {
          inside_superstate.insert(state.getID());
        } else {
          outside_superstate.insert(state.getID());
        }
      }
      if (superstate_to_state_id_map.find())
        superstates_to_process.push(inside_superstate);
      superstates_to_process.push(outside_superstate);
    }
  }
}