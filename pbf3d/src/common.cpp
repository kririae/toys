//
// Created by kr2 on 8/10/21.
//

#include "common.hpp"

glm::vec3 color_ramp(float t, const color &col_left, const color &col_right)
{
  return (1 - t) * col_left + t * col_right;
}
