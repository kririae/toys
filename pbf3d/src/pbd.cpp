//
// Created by kr2 on 8/10/21.
//

#include "pbd.hpp"
#include <cstdio>
#include <iostream>

// static void print(glm::vec3 v) {
//   std::cout << v.x << " " << v.y << " " << v.z << std::endl;
// }

PBDSolver::PBDSolver(float _radius)
    : ch_ptr(std::make_shared<CompactHash>(_radius)), radius(_radius)
{
  mass = 4.0f / 3.0f * glm::pi<float>() * glm::pow(radius, 3.0f);
}

void PBDSolver::set_gui(RTGUI_particles *gui)
{
  gui_ptr = gui;
}

void PBDSolver::callback()
{
  assert(gui_ptr != nullptr);
  const glm::vec3 g(0.0f, 0.0f, 0.0f);

  float rho_0 = 1.0f;
  auto &data = get_data();
  const auto pre_data = data;  // copy

  // Apply forces
  for (auto &i : get_data()) {
    i.v += delta_t * g;
    i.pos += delta_t * i.v;
  }

  // find all neighbors
  ch_ptr->build();

  // Jacobi iteration

  int iter = 8;
  while (iter--) {
    std::vector<float> _lambda, c_i;
    _lambda.reserve(data.size());
    c_i.reserve(data.size());
    double c_i_sum = 0;
    int n_neightbor_sum = 0;

    for (uint i = 0; i < data.size(); ++i) {
      c_i[i] = glm::max(sph_calc_rho(i) / rho_0 - 1, 0.0f);
      c_i_sum += c_i[i];
      n_neightbor_sum += ch_ptr->n_neighbor(i);

      float denom = 100.0f;
      for (int j = 0; j < ch_ptr->n_neighbor(i); ++j) {
        const int neighbor_index = ch_ptr->neighbor(i, j);
        denom += glm::pow(glm::length(grad_c(i, neighbor_index, rho_0)), 2.0f);
      }
      _lambda[i] = -c_i[i] / denom;
      assert(!glm::isnan(_lambda[i]));
      // std::printf(
      //     "lambda: - %f / %f = %f\n", c_i[i], denom, _lambda[i]);
    }

    for (uint i = 0; i < data.size(); ++i) {
      glm::vec3 delta_p_i(0.0f);
      for (int j = 0; j < ch_ptr->n_neighbor(i); ++j) {
        const int neighbor_index = ch_ptr->neighbor(i, j);
        delta_p_i += (_lambda[i] + _lambda[neighbor_index] +
                      compute_s_corr(i, neighbor_index)) *
                     grad_spiky(data[i].pos - data[neighbor_index].pos, radius);
      }

      delta_p_i *= 1.0f / rho_0;
      data[i].pos += delta_p_i;
      data[i].rho = glm::abs(c_i[i]);
      constraint_to_border(data[i]);
    }
    std::cout << "avg constraint: " << c_i_sum / data.size() << " n_neighbor: "
              << static_cast<float>(n_neightbor_sum) / data.size() << std::endl;
  }

  // update all velocity
  for (uint i = 0; i < data.size(); ++i) {
    auto &p = data[i];
    assert(!glm::isnan(p.pos.x));
    assert(!glm::isnan(p.pos.y));
    assert(!glm::isnan(p.pos.z));
    p.v = 1.0f / delta_t * (p.pos - pre_data[i].pos);
  }

  gui_ptr->set_particles(get_data());
}

void PBDSolver::add_particle(const SPHParticle &p)
{
  assert(ch_ptr != nullptr);
  ch_ptr->add_particle(p);
}

void PBDSolver::constraint_to_border(SPHParticle &p) const
{
  // p.pos.x = glm::clamp(p.pos.x, -border, border);
  p.pos.x = 0;
  p.pos.y = glm::clamp(p.pos.y, -border, border);
  p.pos.z = glm::clamp(p.pos.z, -border, border);
  extern Random rd_global;
  p.pos += 1e-3f *
           glm::vec3(rd_global.rand(), rd_global.rand(), rd_global.rand());
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

  assert(!glm::isnan(rho));
  return rho;
}

glm::vec3 PBDSolver::grad_c(int p_i, int p_k, float rho_0)
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

  assert(!glm::isnan(res.x));
  assert(!glm::isnan(res.y));
  assert(!glm::isnan(res.z));
  return 1.0f / rho_0 * res;
}

float PBDSolver::poly6(float r, float d)
{
  float res = 0;
  r = glm::abs(r);
  if (0 <= r && r <= d)
    res = 315.0f / (64 * glm::pi<float>() * glm::pow(d, 9.0f)) *
          glm::pow(d * d - r * r, 3.0f);
  return res;
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
