#include "gui.hpp"
#include <iostream>

constexpr int WIDTH = 800, HEIGHT = 400;

int main()
{
  RTGUI_particles gui;
  gui.main_loop([](){});
  return 0;
}
