//
// Created by kr2 on 8/10/21.
//

#include "pbd.hpp"

PBDSolver::PBDSolver(float _radius)
    : ch_ptr(std::make_shared<CompactHash>(_radius)), radius(_radius)
{
}

void PBDSolver::callback()
{
}

void PBDSolver::add_particle(SPHParticle &&p)
{
  p.set_parent(this);
  ch_ptr->add_particle(std::move(p));
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
