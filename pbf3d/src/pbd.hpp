//
// Created by kr2 on 8/10/21.
//

#ifndef PBF3D_SRC_PBD_HPP_
#define PBF3D_SRC_PBD_HPP_

#include "compact_hash.hpp"
#include "gui.hpp"
#include "solver.hpp"
#include <memory>
#include <vector>

class SPHParticle;

class PBDSolver : public Solver {
 public:
  explicit PBDSolver(float _radius);
  PBDSolver(const PBDSolver &solver) = delete;
  PBDSolver &operator=(const PBDSolver &solver) = delete;
  ~PBDSolver() override = default;

  void set_gui(RTGUI_particles *gui);
  void callback() override;  // gui_ptr required
  void add_particle(const SPHParticle &p);
  void constraint_to_border(SPHParticle &p) const;
  std::vector<SPHParticle> &get_data();

  // PBF mathematics parts...
  float sph_calc_rho(int p_i);
  glm::vec3 grad_c(int p_i, int p_k, float rho_0);
  static float poly6(float r, float d);
  static glm::vec3 grad_spiky(glm::vec3 v, float d);
  float compute_s_corr(int p_i, int p_j);

 private:
  std::shared_ptr<CompactHash> ch_ptr;
  RTGUI_particles *gui_ptr{nullptr};
  float radius, mass{0}, delta_t{1.0 / 60 / 2.0}, border{1.0};
};

#endif  // PBF3D_SRC_PBD_HPP_
