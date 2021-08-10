//
// Created by kr2 on 8/10/21.
//

#ifndef PBF3D_SRC_COMPACT_HASH_HPP_
#define PBF3D_SRC_COMPACT_HASH_HPP_

#include "particle.hpp"

class CompactHash {
  // Currently, a poor implementation
 public:
  explicit CompactHash(float _radius);
  CompactHash(const CompactHash &CH) = delete;
  CompactHash &operator=(const CompactHash &CH) = delete;
  ~CompactHash();

  void add_particle(SPHParticle &&p);
  std::vector<SPHParticle> &get_data();

 private:
  float radius;
  std::vector<SPHParticle> data{};
};
#endif  // PBF3D_SRC_COMPACT_HASH_HPP_
