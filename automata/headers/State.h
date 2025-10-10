#pragma once

using StateID = int;

class State {
private:
  StateID id;

public:
  State(StateID id) : id{id} {}

  StateID getID() const { return id; }
  void setID(StateID id) { this->id = id; }
};