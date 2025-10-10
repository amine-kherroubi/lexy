#include "headers/RegexToNFA.h"
#include "headers/RegexPreprocessor.h"
#include <memory>
#include <stack>

NFA RegexToNFA::buildForSymbol(Symbol c) {
  const States states{State{0}, State{1}};
  const StateIDs accepting_state_ids{1};
  NFA nfa{states, accepting_state_ids, 0};
  nfa.addTransition(0, c, 1);
  return nfa;
}

NFA &RegexToNFA::concatenate(NFA &first_nfa, NFA &second_nfa) {
  int offset = first_nfa.getStates().size();
  States &second_nfa_states = second_nfa.getStates();
  for (State &state : second_nfa_states) {
    state.setID(state.getID() + offset);
  }
  first_nfa.getStates().insert(first_nfa.getStates().end(),
                               second_nfa_states.begin(),
                               second_nfa_states.end());
  first_nfa.resizeTransitions(first_nfa.getStates().size());
  StateIDs old_accepting_ids = first_nfa.getAcceptingStateIDs();
  StateIDs &new_accepting_ids = first_nfa.getAcceptingStateIDs();
  new_accepting_ids.clear();
  for (StateID id : second_nfa.getAcceptingStateIDs()) {
    new_accepting_ids.push_back(id);
  }
  for (const State &state : second_nfa_states) {
    StateID new_from = state.getID();
    StateID original_from = new_from - offset;
    for (Symbol symbol : second_nfa.getSymbols(original_from)) {
      for (StateID to : second_nfa.getNextStates(original_from, symbol)) {
        first_nfa.addTransition(new_from, symbol, offset + to);
      }
    }
    for (StateID to : second_nfa.getEpsilonNextStates(original_from)) {
      first_nfa.addEpsilonTransition(new_from, offset + to);
    }
  }
  StateID second_start = offset + second_nfa.getStartStateID();
  for (StateID accepting : old_accepting_ids) {
    first_nfa.addEpsilonTransition(accepting, second_start);
  }
  return first_nfa;
}

NFA &RegexToNFA::alternate(NFA &first_nfa, NFA &second_nfa) {
  State new_start{0};
  States &first_nfa_states = first_nfa.getStates();
  for (State &state : first_nfa_states) {
    state.setID(state.getID() + 1);
  }
  first_nfa_states.insert(first_nfa_states.begin(), new_start);
  int offset = first_nfa_states.size();
  States &second_nfa_states = second_nfa.getStates();
  for (State &state : second_nfa_states) {
    state.setID(state.getID() + offset);
  }
  first_nfa.getStates().insert(first_nfa.getStates().end(),
                               second_nfa_states.begin(),
                               second_nfa_states.end());
  first_nfa.resizeTransitions(first_nfa.getStates().size());
  StateIDs &accepting_ids = first_nfa.getAcceptingStateIDs();
  for (StateID &id : accepting_ids) {
    id += 1;
  }
  for (StateID id : second_nfa.getAcceptingStateIDs()) {
    accepting_ids.push_back(id);
  }
  StateID old_first_start = first_nfa.getStartStateID() + 1;
  first_nfa.setStartStateID(0);
  first_nfa.addEpsilonTransition(0, old_first_start);
  first_nfa.addEpsilonTransition(0, offset + second_nfa.getStartStateID());
  for (const State &state : second_nfa_states) {
    StateID new_from = state.getID();
    StateID original_from = new_from - offset;
    for (Symbol symbol : second_nfa.getSymbols(original_from)) {
      for (StateID to : second_nfa.getNextStates(original_from, symbol)) {
        first_nfa.addTransition(new_from, symbol, offset + to);
      }
    }
    for (StateID to : second_nfa.getEpsilonNextStates(original_from)) {
      first_nfa.addEpsilonTransition(new_from, offset + to);
    }
  }
  return first_nfa;
}

NFA &RegexToNFA::kleeneStar(NFA &nfa) {
  StateID start_state_id = nfa.getStartStateID();
  StateIDs &accepting_ids = nfa.getAcceptingStateIDs();
  bool start_is_accepting = false;
  for (StateID id : accepting_ids) {
    if (id == start_state_id) {
      start_is_accepting = true;
      break;
    }
  }
  if (!start_is_accepting) {
    accepting_ids.push_back(start_state_id);
  }
  for (StateID accepting_id : accepting_ids) {
    if (accepting_id != start_state_id) {
      nfa.addEpsilonTransition(accepting_id, start_state_id);
    }
  }
  return nfa;
}

NFA RegexToNFA::convert(const std::string &regex) {
  std::string postfix = RegexPreprocessor::preprocess(regex);
  std::stack<std::unique_ptr<NFA>> nfa_stack;
  for (char c : postfix) {
    if (c == '.') {
      std::unique_ptr<NFA> second = std::move(nfa_stack.top());
      nfa_stack.pop();
      std::unique_ptr<NFA> first = std::move(nfa_stack.top());
      nfa_stack.pop();
      concatenate(*first, *second);
      nfa_stack.push(std::move(first));
    } else if (c == '|') {
      std::unique_ptr<NFA> second = std::move(nfa_stack.top());
      nfa_stack.pop();
      std::unique_ptr<NFA> first = std::move(nfa_stack.top());
      nfa_stack.pop();
      alternate(*first, *second);
      nfa_stack.push(std::move(first));
    } else if (c == '*') {
      std::unique_ptr<NFA> nfa = std::move(nfa_stack.top());
      nfa_stack.pop();
      kleeneStar(*nfa);
      nfa_stack.push(std::move(nfa));
    } else {
      nfa_stack.push(std::make_unique<NFA>(buildForSymbol(c)));
    }
  }
  return std::move(*nfa_stack.top());
}