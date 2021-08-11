//
// Created by kr2 on 8/10/21.
//

#include "compact_hash.hpp"
#include "common.hpp"

CompactHash::CompactHash(float _radius) : radius(_radius)
{
}

void CompactHash::add_particle(const SPHParticle &p)
{
  data.push_back(p);
}

std::vector<SPHParticle> &CompactHash::get_data()
{
  return data;
}

int CompactHash::n_points() const
{
  return data.size();
}

int CompactHash::n_neighbor(uint index) const
{
  return neighbor_map[index].size();
}

int CompactHash::neighbor(uint index, uint neighbor_index) const
{
  return neighbor_map[index][neighbor_index];
}

void CompactHash::build()
{
  neighbor_map = std::vector<std::vector<uint>>(n_points());
  for (int i = 0; i < n_points(); ++i) {
    const auto &center = data[i];
    auto &neighbor_map_i = neighbor_map[i];
    for (int j = 0; j < n_points(); ++j) {
      if (center.dist(data[j]) <= radius) {
        neighbor_map_i.push_back(j);
      }
    }
  }
}
