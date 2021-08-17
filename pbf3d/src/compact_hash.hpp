//
// Created by kr2 on 8/10/21.
//

#ifndef PBF3D_SRC_COMPACT_HASH_HPP_
#define PBF3D_SRC_COMPACT_HASH_HPP_

#include "particle.hpp"

constexpr int MAX_NEIGHBOR_SIZE = 60;

class CompactHash {
  // Currently, a poor implementation (for correctness)
 public:
  explicit CompactHash(float _radius);
  CompactHash(const CompactHash &CH) = delete;
  CompactHash &operator=(const CompactHash &CH) = delete;
  ~CompactHash() = default;

  // interface
  void build();
  void add_particle(const SPHParticle &p);
  std::vector<SPHParticle> &get_data();
  [[nodiscard]] int n_points() const;
  [[nodiscard]] int n_neighbor(uint index) const;
  [[nodiscard]] int neighbor(uint index, uint neighbor_index) const;
  std::vector<uint> &neighbor_vec(uint index);

 private:
  const float radius, radius2;
  const int n_grids;
  std::vector<SPHParticle> data{};
  std::vector<std::vector<uint>> neighbor_map{};
  std::vector<std::vector<int>> hash_map{};

  // hash function
  [[nodiscard]] inline int hash(float x, float y, float z) const;
  [[nodiscard]] inline int hash(const glm::vec3 &p) const;
  [[nodiscard]] inline int hash_from_grid(int u, int v, int w) const;
  [[nodiscard]] inline int hash_from_grid(const glm::ivec3 &p) const;
  [[nodiscard]] inline glm::ivec3 get_grid_index(const glm::vec3 &p) const;
};
#endif  // PBF3D_SRC_COMPACT_HASH_HPP_
