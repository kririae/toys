//
// Created by kr2 on 8/9/21.
//

#include "gui.hpp"
#include "common.hpp"
#include "particle.hpp"
#include "shader.hpp"
#include <iostream>

GUI::GUI(int WIDTH, int HEIGHT) : width(WIDTH), height(HEIGHT)
{
  // OpenGL initialization

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 4);

  // assign window
  window = glfwCreateWindow(WIDTH, HEIGHT, "pbf3d", nullptr, nullptr);
  if (window == nullptr) {
    std::cerr << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
  }

  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD" << std::endl;
  }

  glViewport(0, 0, WIDTH, HEIGHT);

  glfwSetFramebufferSizeCallback(
      window, [](GLFWwindow *window, int width, int height) {
        std::clog << "window size changed to " << width << " " << height
                  << std::endl;
        glViewport(0, 0, width, height);
      });
}

GUI::GUI() : GUI(800, 400)
{
}

void GUI::main_loop(const std::function<void()> &callback)
{
  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
}

GUI::~GUI() = default;

RTGUI_particles::RTGUI_particles(int WIDTH, int HEIGHT)
    : GUI::GUI(WIDTH, HEIGHT)
{
}

RTGUI_particles::RTGUI_particles() : RTGUI_particles(800, 400)
{
}

void RTGUI_particles::set_particles(const std::vector<SPHParticle> &_p)
{
  p = _p;

  if (VAO != 0) {
    glDeleteVertexArrays(1, &VAO);
  }

  if (VBO != 0) {
    glDeleteBuffers(1, &VBO);
  }

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  std::vector<float> points;
  points.reserve(p.size() * 4);
  for (auto &i : p) {
    points.push_back(i.x);
    points.push_back(i.y);
    points.push_back(i.z);
    points.push_back((i.y + 1.0f) / 2);
  }

  glBufferData(GL_ARRAY_BUFFER,
               points.size() * sizeof(float),
               points.data(),
               GL_STREAM_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(
      0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void *)nullptr);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(
      1, 1, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void *)(3 * sizeof(float)));

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void RTGUI_particles::main_loop(const std::function<void()> &callback)
{
  // Please call set_particles in callback function and return the newly
  // generated particles

  while (!glfwWindowShouldClose(window)) {
    // Do something with particles
    glClearColor(0.921f, 0.925f, 0.933f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    callback();

    render_particles();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
}

void RTGUI_particles::render_particles() const
{
  auto model = glm::translate(glm::mat4(1.0f), glm::vec3(0));
  model = glm::scale(model, glm::vec3(1.0f));

  auto camera_pos = glm::vec3(1.0f, 3.0f, 6.0f);
  auto camera_center = glm::vec3(0.0f);
  auto camera_up = glm::vec3(0.0f, 1.0f, 0.0f);
  auto view = glm::lookAt(camera_pos, camera_center, camera_up);
  auto projection = glm::perspective(
      glm::radians(45.0f), static_cast<float>(width) / height, 0.1f, 100.0f);

  glBindVertexArray(VAO);

  shader.use();
  shader.set_mat4("model", model);
  shader.set_mat4("view", view);
  shader.set_mat4("projection", projection);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_MULTISAMPLE);
  glPointSize(2);

  glDrawArrays(GL_POINTS, 0, p.size());
  glBindVertexArray(0);
}

void RTGUI_particles::del()
{
  shader.del();
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
}

RTGUI_particles::~RTGUI_particles() = default;
