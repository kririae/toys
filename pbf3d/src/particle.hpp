//
// Created by kr2 on 8/9/21.
//

#ifndef PBF3D__PARTICLE_HPP_
#define PBF3D__PARTICLE_HPP_

#include <memory>
#include <vector>

class PBDSolver;

class Particle {
 public:
  float x, y, z;
  Particle(float _x, float _y, float _z);
  virtual ~Particle() = default;

 protected:
};

class SPHParticle : public Particle {
 public:
  float rho{0};
  SPHParticle(float _x, float _y, float _z);
  SPHParticle(const SPHParticle &p) = default;
  SPHParticle &operator=(const SPHParticle &p) = default;
  ~SPHParticle() override = default;

  void set_parent(PBDSolver *solver);
  float sph_calc_rho();

 private:
  PBDSolver *parent{nullptr};
};

#endif  // PBF3D__PARTICLE_HPP_
