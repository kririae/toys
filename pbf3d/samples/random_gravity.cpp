//
// Created by kr2 on 8/11/21.
//

#include "gui.hpp"
#include "particle.hpp"
#include "pbd.hpp"
#include <iostream>
#include <random>

constexpr int NParticles = 10000;
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

  for (int i = 0; i < NParticles; ++i) {
    pbd.add_particle(SPHParticle(rd_global.rand() * 0.7,
                                 rd_global.rand() * 0.7,
                                 rd_global.rand() * 0.7));
  }

  std::cout << "NParticles: " << NParticles << std::endl;
  // int cnt = 0;
  // for (float x = -0.5; x <= 0.5; x += radius) {
  //   for (float y = -0.5; y <= 0.5; y += radius) {
  //     for (float z = -0.5; z <= 0.5; z += radius) {
  //       ++cnt;
  //       pbd.add_particle(SPHParticle(x, y, z));
  //     }
  //   }
  // }
  // std::cout << "NParticles: " << cnt << std::endl;

  gui.main_loop(callback);
  gui.del();

  return 0;
}
