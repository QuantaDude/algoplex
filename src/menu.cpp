

#include "menu.hpp"
#include "scene.hpp"
#include <cstdint>

Menu::Menu(Font *font) : Scene(font) {}

void Menu::draw(IVector2 *resolution) { Scene::draw(resolution); }

void Menu::update(IVector2 *resolution) { Scene::update(resolution); }

void Menu::init() { Scene::init(); }

void Menu::input() {}

void Menu::resetCameraPos() {}
void Menu::startAlgo() {}
void Menu::stepAlgo() {}
void Menu::resetAlgo() {}
void Menu::resetScene() {}
void Menu::updateMode(int main, int sec) {}
void Menu::setHoverState(bool hover, uint32_t node_id) {}
