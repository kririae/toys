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

using color = glm::vec3;

glm::vec3 color_ramp(float t, const color &col_left, const color &col_right);

#endif  // PBF3D_SRC_COMMON_HPP_
