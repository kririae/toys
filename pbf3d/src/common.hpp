//
// Created by kr2 on 8/10/21.
//

#ifndef PBF3D_SRC_COMMON_HPP_
#define PBF3D_SRC_COMMON_HPP_

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <random>

#define NDEBUG
#define PARALLEL

constexpr float border = 10.0f;
constexpr float epsilon = 1e-5;
constexpr bool rotate = true;
// other pbf parameters are defined in pbd.cpp

using color = glm::vec3;

glm::vec3 color_ramp(float t, const color &col_left, const color &col_right);

class Random {
 public:
  Random();
  float rand();

 private:
  std::random_device rd{};
  std::mt19937 mt;
  std::uniform_real_distribution<float> dist;
};

#endif  // PBF3D_SRC_COMMON_HPP_
