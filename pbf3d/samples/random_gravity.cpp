//
// Created by kr2 on 8/11/21.
//

#include "gui.hpp"
#include "particle.hpp"
#include "pbd.hpp"
#include <iostream>
#include <random>

constexpr int NParticles = 1000;
constexpr int WIDTH = 800, HEIGHT = 600;

int main()
{
  // random generator initialization
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  RTGUI_particles gui(WIDTH, HEIGHT);
  PBDSolver pbd(0.03);
  pbd.set_gui(&gui);
  std::function<void()> callback = [obj = &pbd] { obj->callback(); };

  for (int i = 0; i < NParticles; ++i) {
    pbd.add_particle(SPHParticle(dist(mt), dist(mt), dist(mt)));
  }

  gui.main_loop(callback);
  gui.del();

  return 0;
}
