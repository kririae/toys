//
// Created by kr2 on 8/9/21.
//
#include "particle.hpp"

Particle::Particle(float _x, float _y, float _z) : x(_x), y(_y), z(_z)
{
}

SPHParticle::SPHParticle(float _x, float _y, float _z)
    : Particle::Particle(_x, _y, _z)
{
}

float SPHParticle::sph_calc_rho()
{
  // TODO: `parent` as neighborhood search data structure
  rho = 0.5;
  return rho;
}
