#pragma once
#include "menu.hpp"
#include "raylib.h"
#include "scene.hpp"
#include "scene_registry.hpp"
#include "state.hpp"
#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#if defined(PLATFORM_WEB)
#include "web.hpp"
#include <emscripten.h>
#include <emscripten/html5.h>
#endif
#include "app.hpp"
#include <memory>

App *App::instance = nullptr;
AV::AppState App::g_app_state;

App *App::createInstance(int width, int height) {
  if (instance == nullptr) {
    // if (emscripten_run_preload_plugins(
    //         "/home/abhirup/Documents/algo-visualizer/resources/font/"
    //         "alpha_beta.png",
    //         onPreloadSuccess("font.png"), onPreloadSuccess("font.png"))) {
    //   print_console("sucess loading font");
    // }
    // list_files();

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    emscripten_get_canvas_element_size("#canvas", &width, &height);

    std::string str = std::to_string((int)width);
    print_console(str.c_str());
    InitWindow(width, height, "Algorithm Visualizer");
    SetExitKey(KEY_NULL);
    if (IsWindowReady()) {

      instance = new App(width, height, RESOURCES_PATH "font/alpha_beta.png");
      instance->setState(
          std::make_unique<AV::Scene>(&instance->getDefaultFont()));
      // instance->current_state->init();
#if defined(PLATFORM_WEB)
      // emscripten_get_canvas_element_size("#canvas", &width, &height);
#endif
    }
  }
  return instance;
}

App::App(int width, int height, const char *font)
    : resolution({.x = width, .y = height}), g_font(LoadFont(font)) {
  g_app_state = AV::SCENE;
  // createInstance(width, height);
}

App &App::getInstance() {
  if (instance == nullptr) {
    instance = new App(1280, 720);
  }
  return *instance;
}
Font &App::getDefaultFont() { return this->g_font; }

IVector2 *App::getResolution() { return &resolution; }

void App::setState(std::unique_ptr<AV::State> state) {
  current_state = std::move(state);
  // current_state->init();
}
void App::setState(AV::AppState new_state) { g_app_state = new_state; }

void App::run(void) {
  if (App::g_app_state != AV::QUIT) {
    current_state->input();
    current_state->update();
    current_state->draw(&resolution);
  } else {
    // this->setState(std::make_unique<Menu>("Algorithm Visualizer"));
    App::shutdown();
  }
}
void App::shutdown() {

  current_state.reset();

  UnloadFont(g_font);

  delete instance;
  instance = nullptr;

  CloseWindow();
#if defined(PLATFORM_WEB)

  emscripten_pause_main_loop();
  emscripten_cancel_main_loop();
  emscripten_exit_with_live_runtime();
  // emscripten_force_exit(0);

#endif
}

void App::runWrapper() { getInstance().run(); }

#if defined(PLATFORM_WEB)
void App::initWeb() {
  emscripten_get_canvas_element_size("#canvas", &resolution.x, &resolution.y);
  current_state->init();

  notify_algorithms();
  emscripten_set_main_loop(runWrapper, 60, 1);
}

extern "C" void notify_algorithms() {
  std::string json = "[";

  for (uint16_t i = 0; i < ALGORITHM_COUNT; i++) {
    const auto &a = ALGORITHMS[i];

    if (i > 0)
      json += ",";
    json += "{";
    json += "\"id\":\"" +
            std::string(std::to_string(std::to_underlying(a.id))) + "\",";
    json += "\"name\":\"" + std::string(a.name) + "\",";
    json += "\"category\":\"" +
            std::string(std::to_string(std::to_underlying(a.category))) + "\",";
    json += "\"subCategories\":\"" + std::string(a.subCategories) + "\"";
    json += "}";
  }
  json += "]";
  js_register_algorithms(json.c_str());
}
#endif
