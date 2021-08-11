//
// Created by kr2 on 8/10/21.
//

#include "pbd.hpp"
#include <iostream>

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

    constraint_to_border(i);
  }

  ch_ptr->build();
  auto &data = ch_ptr->get_data();
  for (uint i = 0; i < ch_ptr->n_points(); ++i) {
    auto &_p = data[i];
    _p.rho = glm::abs(_p.v.y) / 5.0;
    // for (int j = ch_ptr->n_neighbor(i) - 1; j >= 0; --j) {
    //   _p.rho += PBDSolver::poly6(_p.dist(data[j]), radius);
    // }
    // std::cout << _p.rho << std::endl;
  }

  gui_ptr->set_particles(get_data());
}

void PBDSolver::add_particle(const SPHParticle &p)
{
  assert(ch_ptr != nullptr);
  ch_ptr->add_particle(p);
}

void PBDSolver::constraint_to_border(SPHParticle &p) const
{
  p.pos.x = glm::clamp(p.pos.x, -border, border);
  p.v.x = glm::abs(p.pos.x) == border ? 0.0 : p.v.x;
  p.pos.y = glm::clamp(p.pos.y, -border, border);
  p.v.y = glm::abs(p.pos.y) == border ? 0.0 : p.v.y;
  p.pos.z = glm::clamp(p.pos.z, -border, border);
  p.v.z = glm::abs(p.pos.z) == border ? 0.0 : p.v.z;
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
