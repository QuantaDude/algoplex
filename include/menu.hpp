#pragma once
#include "app.hpp"
#include "raylib.h"
#include "scene.hpp"

class Menu : public Scene {

public:
  Menu(Font *);
  void init() override;
  void draw(IVector2 *) override;
  void update(IVector2 *) override;
  void input() override;
  void DrawUI(IVector2);
};
