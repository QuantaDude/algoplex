

#include "menu.hpp"
#include "scene.hpp"

Menu::Menu(Font *font) : Scene(font) {}

void Menu::draw(IVector2 *resolution) { Scene::draw(resolution); }

void Menu::update(IVector2 *resolution) { Scene::update(resolution); }

void Menu::init() { Scene::init(); }

void Menu::input() {}
