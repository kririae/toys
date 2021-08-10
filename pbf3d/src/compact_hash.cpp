//
// Created by kr2 on 8/10/21.
//

#include "compact_hash.hpp"

CompactHash::CompactHash(float _radius) : radius(_radius)
{
}

void CompactHash::add_particle(SPHParticle &&p)
{
  data.push_back(p);
}

std::vector<SPHParticle> &CompactHash::get_data()
{
  return data;
}

CompactHash::~CompactHash() = default;
