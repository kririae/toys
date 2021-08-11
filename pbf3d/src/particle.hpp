//
// Created by kr2 on 8/9/21.
//

#ifndef PBF3D__PARTICLE_HPP_
#define PBF3D__PARTICLE_HPP_

#include "common.hpp"
#include <memory>
#include <vector>

class Particle {
 public:
  glm::vec3 pos;
  Particle(glm::vec3 _pos);
  Particle(float x, float y, float z);
  virtual ~Particle() = default;
  float dist(const Particle &p) const;

 protected:
};

class SPHParticle : public Particle {
 public:
  float rho{0};
  glm::vec3 v{0};
  SPHParticle(glm::vec3 _pos);
  SPHParticle(float x, float y, float z);
  SPHParticle(const SPHParticle &p) = default;
  SPHParticle &operator=(const SPHParticle &p) = default;
  ~SPHParticle() override = default;

 private:
};

#endif  // PBF3D__PARTICLE_HPP_
