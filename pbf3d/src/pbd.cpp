//
// Created by kr2 on 8/10/21.
//

#include "pbd.hpp"
#include <chrono>
#include <cstdio>
#include <iostream>

// static void print(glm::vec3 v) {
//   std::cout << v.x << " " << v.y << " " << v.z << std::endl;
// }

constexpr float rho_0 = 10.0f;
constexpr float denom_epsilon = 20.0f;
constexpr int iter = 3;

PBDSolver::PBDSolver(float _radius)
    : ch_ptr(std::make_shared<CompactHash>(_radius)), radius(_radius)
{
  mass = 4.0f / 3.0f * glm::pi<float>() * radius * radius;
}

void PBDSolver::set_gui(RTGUI_particles *gui)
{
  gui_ptr = gui;
}

void PBDSolver::callback()
{
  auto start = std::chrono::system_clock::now();
  assert(gui_ptr != nullptr);

  auto &data = get_data();
  const auto pre_data = data;  // copy
  const int data_size = data.size();
  const glm::vec3 g(0.0f, -9.8f, 0.0f);

  // Apply forces
  for (auto &i : data) {
    i.v += delta_t * g;
    i.pos += delta_t * i.v;
    constraint_to_border(i);
  }

  // find all neighbors
  ch_ptr->build();

  // Jacobi iteration
  double c_i_sum = 0;
  long long n_neightbor_sum = 0;

  int iter_cnt = iter;
  while (iter_cnt--) {
    std::vector<float> _lambda, c_i;
    _lambda.reserve(data.size());
    c_i.reserve(data.size());

#pragma omp parallel for default(none) \
    shared(data_size, c_i, c_i_sum, n_neightbor_sum, _lambda)
    for (int i = 0; i < data_size; ++i) {
      // Basic calculation
      const float rho = sph_calc_rho(i);
      c_i[i] = glm::max(rho / rho_0 - 1, 0.0f);
      c_i_sum += c_i[i];
      n_neightbor_sum += ch_ptr->n_neighbor(i);

      float _denom = 0.0f;
      const auto &neighbor_vec = ch_ptr->neighbor_vec(i);
      for (const auto &j : neighbor_vec)
        _denom += glm::pow(glm::length(grad_c(i, j)), 2.0f);

      _lambda[i] = -c_i[i] / (_denom + denom_epsilon);
    }

#pragma omp parallel for default(none) shared(data_size, _lambda, data, c_i)
    for (int i = 0; i < data_size; ++i) {
      glm::vec3 delta_p_i(0.0f);
      const auto &neighbor_vec = ch_ptr->neighbor_vec(i);
      for (const auto &j : neighbor_vec) {
        delta_p_i += (_lambda[i] + _lambda[j] + compute_s_corr(i, j)) *
                     grad_spiky(data[i].pos - data[j].pos, radius);
      }

      delta_p_i *= 1.0f / rho_0;
      data[i].pos += delta_p_i;
      data[i].rho = glm::clamp(c_i[i], 0.0f, 1.0f);

      constraint_to_border(data[i]);
    }
  }

  std::cout << "avg c_i: " << c_i_sum / data_size / iter << " | n_neighbor: "
            << static_cast<float>(n_neightbor_sum) / data_size / iter
            << std::endl;

  // update all velocity
  for (int i = 0; i < data_size; ++i) {
    auto &p = data[i];
    p.v = 1.0f / delta_t * (p.pos - pre_data[i].pos);
  }

  gui_ptr->set_particles(get_data());

  auto end = std::chrono::system_clock::now();
  std::chrono::duration<float> diff = end - start;
  std::cout << "calculation complete: " << diff.count() * 1000 << "ms"
            << std::endl;
}

void PBDSolver::add_particle(const SPHParticle &p)
{
  assert(ch_ptr != nullptr);
  ch_ptr->add_particle(p);
}

void PBDSolver::constraint_to_border(SPHParticle &p)
{
  extern Random rd_global;
  p.pos += epsilon *
           glm::vec3(rd_global.rand(), rd_global.rand(), rd_global.rand());
  p.pos.x = glm::clamp(p.pos.x, -border, border);
  p.pos.y = glm::clamp(p.pos.y, -border, border);
  p.pos.z = glm::clamp(p.pos.z, -border, border);
}

float PBDSolver::sph_calc_rho(int p_i)
{
  float rho = 0;
  const auto &data = get_data();
  for (int i = 0; i < ch_ptr->n_neighbor(p_i); ++i) {
    const int neighbor_index = ch_ptr->neighbor(p_i, i);
    rho += mass *
           poly6(glm::length(data[p_i].pos - data[neighbor_index].pos), radius);
  }
  return rho;
}

glm::vec3 PBDSolver::grad_c(int p_i, int p_k)
{
  // Assume CH is built
  const auto &data = get_data();
  glm::vec3 res(0.0f);
  if (p_i == p_k) {
    for (int i = 0; i < ch_ptr->n_neighbor(p_i); ++i) {
      const int neighbor_index = ch_ptr->neighbor(p_i, i);
      res += grad_spiky(data[p_i].pos - data[neighbor_index].pos, radius);
    }
  }
  else {
    res = -grad_spiky(data[p_i].pos - data[p_k].pos, radius);
  }
  return 1.0f / rho_0 * res;
}

float PBDSolver::poly6(float r, float d)
{
  r = glm::clamp(glm::abs(r), 0.0f, d);
  const float t = (d * d - r * r) / (d * d * d);
  return 315.0f / (64 * glm::pi<float>()) * t * t * t;
}

glm::vec3 PBDSolver::grad_spiky(glm::vec3 v, float d)
{
  float len = glm::length(v);
  glm::vec3 res(0.0f);
  if (0 < len && len <= d)
    res = static_cast<float>(-45 / (glm::pi<float>() * glm::pow(d, 6)) *
                             glm::pow(d - len, 2)) *
          glm::normalize(v);
  return res;
}

float PBDSolver::compute_s_corr(int p_i, int p_j)
{
  float k = 0.1f;  // k
  float n = 4.0f;
  float delta_q = 0.3f * radius;
  const auto &data = get_data();
  float r = glm::length(data[p_i].pos - data[p_j].pos);
  return -k * glm::pow(poly6(r, radius) / poly6(delta_q, radius), n);
}

std::vector<SPHParticle> &PBDSolver::get_data()
{
  return ch_ptr->get_data();
}
