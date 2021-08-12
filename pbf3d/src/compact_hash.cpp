//
// Created by kr2 on 8/10/21.
//

#include "compact_hash.hpp"
#include "common.hpp"
#include "omp.h"
#include <chrono>
#include <iostream>

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
  std::cout << "constructing hash map" << std::endl;
  auto start = std::chrono::system_clock::now();

  for (auto &i : hash_map)
    i.clear();

  for (uint i = 0; i < data.size(); ++i) {
    const auto &p = data[i];
    const int hash_map_index = hash(p.pos);
    hash_map[hash_map_index].push_back(i);
  }

  const static glm::vec3 x(1.0f, 0.0f, 0.0f);
  const static glm::vec3 y(0.0f, 1.0f, 0.0f);
  const static glm::vec3 z(0.0f, 0.0f, 1.0f);

  int max_n_neighbor = 0;

  for (uint i = 0; i < data.size(); ++i) {
    const auto &center = data[i];

    auto update_neighbor_map = [&](glm::vec3 pos) {
      // calculate pos (in grid coordinate)
      const int _hash_map_index = hash(pos);
      // if (pos.x < -border || pos.x > border || pos.y < -border ||
      //     pos.y > border || pos.z < -border || pos.z > border)
      //   return;
      for (auto j : hash_map[_hash_map_index]) {
        if (glm::length(center.pos - data[j].pos) <= radius) {
          neighbor_map[i].push_back(j);
          max_n_neighbor = glm::max(max_n_neighbor,
                                    static_cast<int>(neighbor_map[i].size()));
        }
      }
    };

    update_neighbor_map(center.pos);

    // 6
    update_neighbor_map(center.pos + x * radius);
    update_neighbor_map(center.pos - x * radius);
    update_neighbor_map(center.pos + y * radius);
    update_neighbor_map(center.pos - y * radius);
    update_neighbor_map(center.pos + z * radius);
    update_neighbor_map(center.pos - z * radius);
    update_neighbor_map(center.pos - z * radius);

    // 12
    update_neighbor_map(center.pos + (x + y) * radius);
    update_neighbor_map(center.pos - (x + y) * radius);
    update_neighbor_map(center.pos + (x + z) * radius);
    update_neighbor_map(center.pos - (x + z) * radius);
    update_neighbor_map(center.pos + (y + z) * radius);
    update_neighbor_map(center.pos - (y + z) * radius);
    update_neighbor_map(center.pos + (x - y) * radius);
    update_neighbor_map(center.pos - (x - y) * radius);
    update_neighbor_map(center.pos + (x - z) * radius);
    update_neighbor_map(center.pos - (x - z) * radius);
    update_neighbor_map(center.pos + (y - z) * radius);
    update_neighbor_map(center.pos - (y - z) * radius);

    // 8
    update_neighbor_map(center.pos + (x + y + z) * radius);
    update_neighbor_map(center.pos + (x + y - z) * radius);
    update_neighbor_map(center.pos + (x - y + z) * radius);
    update_neighbor_map(center.pos + (x - y - z) * radius);
    update_neighbor_map(center.pos - (x + y + z) * radius);
    update_neighbor_map(center.pos - (x + y - z) * radius);
    update_neighbor_map(center.pos - (x - y + z) * radius);
    update_neighbor_map(center.pos - (x - y - z) * radius);

    // 27
  }

  auto end = std::chrono::system_clock::now();
  std::chrono::duration<float> diff = end - start;
  std::cout << "max neighbor size: " << max_n_neighbor << std::endl;
  std::cout << "construction complete: " << diff.count() * 1000 << "ms"
            << std::endl;
}

int CompactHash::hash(float x, float y, float z)
{
  static long long p1 = 73856093, p2 = 19349663, p3 = 83492791;
  const int res = (static_cast<long long>(glm::floor(x / radius) * p1) ^
                   static_cast<long long>(glm::floor(y / radius) * p2) ^
                   static_cast<long long>(glm::floor(z / radius) * p3)) %
                  MOD;
  return (res + MOD) % MOD;
}

int CompactHash::hash(glm::vec3 p)
{
  return hash(p.x, p.y, p.z);
}
