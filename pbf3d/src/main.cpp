#include "common.hpp"
#include "gui.hpp"
#include "particle.hpp"
#include <iostream>
#include <random>

constexpr int NParticles = 6000;
constexpr int WIDTH = 800, HEIGHT = 600;

int main()
{
  // random generator initialization
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_real_distribution<float> dist(-1.0, 1.0);

  static std::vector<SPHParticle> p;
  p.reserve(NParticles);
  for (int i = 0; i < NParticles; ++i) {
    p.emplace_back(dist(mt), dist(mt), dist(mt));
  }

  static RTGUI_particles gui(WIDTH, HEIGHT);
  gui.main_loop([&]() { gui.set_particles(p); });
  gui.del();

  return 0;
}
