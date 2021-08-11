//
// Created by kr2 on 8/9/21.
//
#include "particle.hpp"
#include "common.hpp"
#include <memory>

Particle::Particle(glm::vec3 _pos) : pos(_pos)
{
}

Particle::Particle(float x, float y, float z) : pos(x, y, z)
{
}

float Particle::dist(const Particle &p) const
{
  return glm::sqrt(glm::pow(pos.x - p.pos.x, 2) + glm::pow(pos.y - p.pos.y, 2) +
                   glm::pow(pos.z - p.pos.z, 2));
}

SPHParticle::SPHParticle(glm::vec3 _pos) : Particle::Particle(_pos)
{
}

SPHParticle::SPHParticle(float x, float y, float z) : Particle(x, y, z)
{
}
