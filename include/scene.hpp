#pragma once
#include "raylib.h"
#include "utils.hpp"
typedef enum {
  MENU = 0,
  SCENE,
  GRAPH_SCENE,
  QUIT
} AppState;
typedef enum AlgorithmState {
  Idle,
  Stepping,
  Running,
  Done
} AlgorithmState;
// enum class AlgorithmState {Idle, Stepping, Running, Done};
class Scene {
  AppState state;

public:
  Font *m_font;
  Camera2D g_camera;
  Scene();
  Scene(Font *);
  virtual ~Scene() {}
  virtual void init() = 0;
  virtual void draw(IVector2 *) = 0;
  virtual void update(IVector2 *) = 0;
  virtual void input() {};
};
