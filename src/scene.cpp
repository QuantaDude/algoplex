#include "scene.hpp"

#include "app.hpp"
#include "colors.h"
#include "raylib.h"
#include <cstdio>

Scene::Scene() : g_camera({{0}}) {}

Scene::Scene(Font *font) : m_font(font), g_camera({{0}}) {}

void Scene::init() {

  IVector2 *resolution = App::m_GetInstance().m_GetResolution();
  g_camera.target = {0, 0};
  g_camera.offset = {resolution->x * 0.5f, resolution->y * 0.5f};
  g_camera.zoom = 1.5f;
}

void Scene::draw(IVector2 *resolution) {
  char text[64];

  snprintf(text, 64, "Select an algorithm in the sidebar.");
  Vector2 strWidth = MeasureTextEx(*m_font, text, 25, 2);
  DrawTextEx(
      *m_font, text,
      {(resolution->x - strWidth.x) / 2, static_cast<float>(resolution->y / 2)},
      25, 2, COLOR_TEXT);
}

void Scene::update(IVector2 *resolution) {}
