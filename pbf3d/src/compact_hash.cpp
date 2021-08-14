//
// Created by kr2 on 8/10/21.
//

#include "compact_hash.hpp"
#include "common.hpp"
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
  const int n_grids = glm::ceil(border * 2 / radius) + 1;
  const int data_size = data.size();
  auto start = std::chrono::system_clock::now();

  for (auto &i : hash_map)
    i.clear();

  // initialize the hash_map
  for (int i = 0; i < data_size; ++i)
    hash_map[hash(data[i].pos)].push_back(i);

  int max_n_neighbor = 0;
  for (int i = 0; i < data_size; ++i) {
    const auto &center = data[i];
    const auto &grid_index = get_grid_index(center.pos);

    for (int u = grid_index.x - 1; u <= grid_index.x + 1; ++u)
      for (int v = grid_index.y - 1; v <= grid_index.y + 1; ++v)
        for (int w = grid_index.z - 1; w <= grid_index.z + 1; ++w) {
          if (u < 0 || v < 0 || w < 0 || u > n_grids || v > n_grids ||
              w > n_grids)  // TODO: implement AABB
            continue;

          const int _hash_index = hash_from_grid(u, v, w);
          for (auto &j : hash_map[_hash_index]) {
            if (center.dist(data[j]) <= radius) {
              neighbor_map[i].push_back(j);
              max_n_neighbor = glm::max(static_cast<ulong>(max_n_neighbor),
                                        neighbor_map[i].size());
            }
          }
        }
  }

  for (auto &i : neighbor_map) {
    // To avoid hash conflict
    std::sort(i.begin(), i.end());
    auto end_unique = std::unique(i.begin(), i.end());
    i.erase(end_unique, i.end());
  }

  auto end = std::chrono::system_clock::now();
  std::chrono::duration<float> diff = end - start;
  std::cout << "max neighbor size: " << max_n_neighbor << std::endl;
  std::cout << "construction complete: " << diff.count() * 1000 << "ms"
            << std::endl;
}

int CompactHash::hash(float x, float y, float z) const
{
  static long long p1 = 73856093, p2 = 19349663, p3 = 83492791;
  const auto &grid_index = get_grid_index(glm::vec3(x, y, z));
  const long long res = (static_cast<long long>(grid_index.x) * p1 ^
                         static_cast<long long>(grid_index.y) * p2 ^
                         static_cast<long long>(grid_index.z) * p3) %
                        MOD;
  return (res + MOD) % MOD;  // correct to positive
}

int CompactHash::hash(const glm::vec3 &p)
{
  return hash(p.x, p.y, p.z);
}

glm::ivec3 CompactHash::get_grid_index(const glm::vec3 &p) const
{
  int u = static_cast<int>(glm::floor((p.x + border + 1e-4) / radius));
  int v = static_cast<int>(glm::floor((p.y + border + 1e-4) / radius));
  int w = static_cast<int>(glm::floor((p.z + border + 1e-4) / radius));
  return {u, v, w};
}

int CompactHash::hash_from_grid(int u, int v, int w)
{
  static long long p1 = 73856093, p2 = 19349663, p3 = 83492791;
  const auto &grid_index = glm::ivec3(u, v, w);
  // assert(u >= 0 && v >= 0 && w >= 0);
  const long long res = (static_cast<long long>(grid_index.x) * p1 ^
                         static_cast<long long>(grid_index.y) * p2 ^
                         static_cast<long long>(grid_index.z) * p3) %
                        MOD;
  return (res + MOD) % MOD;  // correct to positive
}

int CompactHash::hash_from_grid(const glm::ivec3 &p)
{
  return hash_from_grid(p.x, p.y, p.z);
}

std::vector<uint> &CompactHash::neighbor_vec(uint index)
{
  // the result should not be changed
  return neighbor_map[index];
}
