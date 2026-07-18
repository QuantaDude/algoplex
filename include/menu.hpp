#pragma once
#include "app.hpp"
#include "raylib.h"
#include "state.hpp"

class Menu : public State {
  const char *m_title;
  bool startScene = false;
  Font &m_font;

public:
  Menu(const char *);
  void init() override;
  void draw(IVector2 *) override;
  void update(IVector2 *) override;
  void input() override;
  void DrawUI(IVector2);

  void calcGOL(IVector2 *);
  void drawCells(IVector2 *);
};
