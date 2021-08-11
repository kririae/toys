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

uint CompactHash::n_points() const
{
  return data.size();
}

uint CompactHash::n_neighbor(uint index) const
{
  const auto &center = data[index];
  return std::count_if(data.cbegin(), data.cend(), [&](const auto &i) {
    return center.dist(i) <= radius;
  });
}

uint CompactHash::neighbor(uint index, uint neighbor_index) const
{
  const auto &center = data[index];
  for (uint i = 0; i < n_points(); ++i) {
    if (center.dist(data[i]) <= radius) {
      if (neighbor_index == 0)
        return i;
      else
        --neighbor_index;
    }
  }

  // should not reach here
  assert(false);
}

void CompactHash::build()
{
  // data.clear();
}
