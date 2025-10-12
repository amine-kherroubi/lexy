#pragma once

using StateID = int;

class State {
private:
  StateID id_;

public:
  State(StateID id) : id_(id) {}

  StateID getID() const { return id_; }
  void setID(StateID id) { this->id_ = id; }
};