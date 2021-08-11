//
// Created by kr2 on 8/10/21.
//

#include "pbd.hpp"

PBDSolver::PBDSolver(float _radius)
    : ch_ptr(std::make_shared<CompactHash>(_radius)), radius(_radius)
{
}

void PBDSolver::set_gui(RTGUI_particles *gui)
{
  gui_ptr = gui;
}

void PBDSolver::callback()
{
  assert(gui_ptr != nullptr);
  const glm::vec3 g(0.0f, -9.8f, 0.0f);
  for (auto &i : ch_ptr->get_data()) {
    i.v += delta_t * g;
    i.pos += delta_t * i.v;

    if (i.pos.y <= -1) {
      i.pos.y = -1;
      i.v.y = 0;
    }
  }
  gui_ptr->set_particles(get_data());
}

void PBDSolver::add_particle(const SPHParticle &p)
{
  assert(ch_ptr != nullptr);
  ch_ptr->add_particle(p);
}

float PBDSolver::poly6(float r, float d)
{
  if (0.0f <= r && r <= d) {
    return 315.0f / (64 * glm::pi<float>() * glm::pow(d, 9.0f)) *
           glm::pow(d * d - r * r, 3.0f);
  }
  else {
    return 0.0f;
  }
}

std::vector<SPHParticle> &PBDSolver::get_data()
{
  return ch_ptr->get_data();
}
