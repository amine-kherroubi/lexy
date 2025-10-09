using StateID = int;

class State {
private:
  StateID id;

public:
  State(StateID id) : id{id} {}
  int getId() const { return id; }
};