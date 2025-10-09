using StateID = int;

class State {
private:
  StateID id;

public:
  State(StateID id) : id{id} {}
  StateID getId() const { return id; }
};