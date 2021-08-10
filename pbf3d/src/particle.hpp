//
// Created by kr2 on 8/9/21.
//

#ifndef PBF3D__PARTICLE_HPP_
#define PBF3D__PARTICLE_HPP_

#include <memory>
#include <vector>

class Particle {
 public:
  float x, y, z;
  Particle(float _x, float _y, float _z);
  virtual ~Particle();

 protected:
};

class SPHParticle : public Particle {
 public:
  float rho{0};
  SPHParticle(float _x, float _y, float _z);
  virtual float sph_calc_rho();
  ~SPHParticle() override;

 protected:
  std::shared_ptr<std::vector<SPHParticle>> parent{};
};

#endif  // PBF3D__PARTICLE_HPP_
