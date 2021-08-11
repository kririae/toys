//
// Created by kr2 on 8/11/21.
//

#include "common.hpp"
#include "compact_hash.hpp"
#include "gtest/gtest.h"
#include <random>

static glm::vec3 gen_particle(glm::vec3 center, float radius)
{
  return glm::vec3(radius, 0.0f, 0.0f) + center;
}

TEST(COMPACT_HASH_CORRECTNESS, n_points)
{
  float radius = 0.03f;
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_real_distribution<float> dist(-radius, radius);

  CompactHash ch(radius);
  auto center = glm::vec3(0.0f);
  ch.add_particle(SPHParticle(center));
  EXPECT_EQ(ch.n_points(), 1);

  uint n_neighbor = 20;
  for (uint i = 0; i < n_neighbor; ++i) {
    ch.add_particle(gen_particle(center, radius));
  }

  EXPECT_EQ(ch.n_points(), n_neighbor + 1);
  EXPECT_EQ(ch.n_neighbor(0), n_neighbor + 1);
}