//
// Created by kr2 on 8/10/21.
//

#ifndef PBF3D_SRC_COMPACT_HASH_HPP_
#define PBF3D_SRC_COMPACT_HASH_HPP_

#include "particle.hpp"
#include <map>

static const int MOD{10007};

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
  int hash(float x, float y, float z);
  int hash(glm::vec3 p);
  int n_points() const;
  int n_neighbor(uint index) const;
  int neighbor(uint index, uint neighbor_index) const;

 private:
  float radius, border{1.0f};
  std::vector<SPHParticle> data{};
  std::vector<std::vector<uint>> neighbor_map{};
  std::vector<int> hash_map[MOD]{};
};
#endif  // PBF3D_SRC_COMPACT_HASH_HPP_
