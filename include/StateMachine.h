#include <List.h>

#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

class State {
  public:
  State(){};
  ~State(){};
  virtual void init();
  virtual void update();
  virtual void draw();



  void nextState(int nextState) {
    _nextState = nextState;
    exit = true;
  }

  int index;
  int _nextState;
  bool exit = false;
 
};


class StateMachine {
  public:
  StateMachine() { _states = new List<State *>(); };
  ~StateMachine(){};

  List<State *> *_states;
  bool isRunning = false;
  int currentState = -1;
  int lastState;

  void initialState(int state){currentState = state;}
  void begin() {
    if (_states->size() == 0)
      return;
    if (currentState == -1)
      currentState = 0;
    isRunning = true;
  }

  void run() {
    lastState = currentState;
    _states->get(currentState)->init();
    while (isRunning) {
      _states->get(currentState)->update();
      _states->get(currentState)->draw();

      if (_states->get(currentState)->exit) {
        currentState = _states->get(currentState)->_nextState;
        isRunning = false;
      }
    }
    _states->get(lastState)->exit = false;
    isRunning = true;
  }

  State *add(State *state) {
    _states->push(state);
    state->index = _states->size() - 1;
    return state;
  }
};

#endif