using StateID = int;

class State {
private:
  StateID id;
  bool is_accepting;

public:
  State(StateID id, bool is_accepting = false)
      : id{id}, is_accepting{is_accepting} {}

  int getId() const { return id; }
  bool isAccepting() const { return is_accepting; }
};