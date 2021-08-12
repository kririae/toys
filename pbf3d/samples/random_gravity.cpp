//
// Created by kr2 on 8/11/21.
//

#include "gui.hpp"
#include "particle.hpp"
#include "pbd.hpp"
#include <iostream>
#include <random>

constexpr int NParticles = 800;
constexpr int WIDTH = 800, HEIGHT = 600;
constexpr float radius = 0.03;

Random rd_global;
int main()
{
  // random generator initialization

  RTGUI_particles gui(WIDTH, HEIGHT);
  PBDSolver pbd(radius);
  pbd.set_gui(&gui);
  std::function<void()> callback = [obj = &pbd] { obj->callback(); };

  int cnt = 0;
  // for (float x = -0.5; x <= 0.5; x += 2 * radius) {
  for (float y = -0.5; y <= 0.5; y += 2 * radius * 0.3) {
    for (float z = -0.5; z <= 0.5; z += 2 * radius * 0.3) {
      ++cnt;
      pbd.add_particle(SPHParticle(0, y, z));
    }
  }
  // }
  std::cout << "NParticles: " << cnt << std::endl;

  gui.main_loop(callback);
  gui.del();

  return 0;
}
