#pragma once
#include "utils.hpp"

typedef enum {
  MENU = 0,
  SCENE,
  QUIT
} AppState;
typedef enum AlgorithmState {
  Idle,
  Stepping,
  Running,
  Done
} AlgorithmState;
// enum class AlgorithmState {Idle, Stepping, Running, Done};
class State {
  AppState state;

public:
  virtual ~State() {}
  virtual void init() = 0;
  virtual void draw(IVector2 *) = 0;
  virtual void update(IVector2 *) = 0;
  virtual void input() {};
};
