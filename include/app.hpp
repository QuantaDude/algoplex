#pragma once
#include "arena.hpp"
#include "raylib.h"
#include "scene.hpp"
#include "scene_registry.hpp"
#include "state.hpp"
#include <cstdlib>
#include <memory>
#if defined(PLATFORM_WEB)
#include "web.hpp"
#endif

struct App {
  Arena m_arena;

  static App *instance;
  AV::State *current_scene;

  IVector2 m_resolution;
  Font m_font;
  AV::AppState m_app_state;

  bool m_mouse_hovering = true;

  void m_LoadScene();
  void m_Run();
  void m_Shutdown();

  static void m_RunWrapper();
  static App &m_GetInstance();
  IVector2 *m_GetResolution();
  App() {}
  App(IVector2 resolution, const char *font = "");
  ~App();
  App(const App &) = delete;
  App &operator=(const App &) = delete;
};

void initApp(App *app, IVector2 resolution, const char *font = "");

extern "C" void set_receive_inputs(bool);
extern "C" void notify_algorithms();
