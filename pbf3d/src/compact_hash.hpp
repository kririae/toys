//
// Created by kr2 on 8/10/21.
//

#ifndef PBF3D_SRC_COMPACT_HASH_HPP_
#define PBF3D_SRC_COMPACT_HASH_HPP_

#include "particle.hpp"
#include <map>

class CompactHash {
  // Currently, a poor implementation (for correctness)
 public:
  explicit CompactHash(float _radius);
  CompactHash(const CompactHash &CH) = delete;
  CompactHash &operator=(const CompactHash &CH) = delete;
  ~CompactHash() = default;

  void build();
  void add_particle(const SPHParticle &p);
  std::vector<SPHParticle> &get_data();
  uint n_points() const;
  uint n_neighbor(uint index) const;
  uint neighbor(uint index, uint neighbor_index) const;

 private:
  float radius;
  std::vector<SPHParticle> data{};
};
#endif  // PBF3D_SRC_COMPACT_HASH_HPP_
