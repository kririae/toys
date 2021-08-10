//
// Created by kr2 on 8/10/21.
//

#ifndef PBF3D_SRC_PBD_HPP_
#define PBF3D_SRC_PBD_HPP_

#include "compact_hash.hpp"
#include "gui.hpp"
#include <memory>
#include <vector>

class SPHParticle;

class PBDSolver {
 public:
  explicit PBDSolver(float _radius);
  PBDSolver(const PBDSolver &solver) = delete;
  PBDSolver &operator=(const PBDSolver &solver) = delete;
  ~PBDSolver() = default;

  void callback();  // gui_ptr required
  void add_particle(SPHParticle &&p);
  static float poly6(float r, float d);
  std::vector<SPHParticle> &get_data();

 private:
  std::shared_ptr<CompactHash> ch_ptr;
  std::shared_ptr<GUI> gui_ptr{nullptr};
  float radius{0.03};
};

#endif  // PBF3D_SRC_PBD_HPP_
