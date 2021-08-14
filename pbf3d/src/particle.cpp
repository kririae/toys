//
// Created by kr2 on 8/9/21.
//

#include "particle.hpp"
#include "common.hpp"

Particle::Particle(glm::vec3 _pos) : pos(_pos)
{
}

Particle::Particle(float x, float y, float z) : pos(x, y, z)
{
}

float Particle::dist(const Particle &p) const
{
  return glm::length(pos - p.pos);
}

SPHParticle::SPHParticle(glm::vec3 _pos) : Particle::Particle(_pos)
{
}

SPHParticle::SPHParticle(float x, float y, float z) : Particle(x, y, z)
{
}
