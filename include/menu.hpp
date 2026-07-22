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

  void resetScene() override;
  void updateMode(int, int) override;
  void setHoverState(bool, uint32_t) override;
  void resetCameraPos() override;
  void startAlgo() override;
  void stepAlgo() override;
  void resetAlgo() override;
};
