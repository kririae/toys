//
// Created by kr2 on 8/9/21.
//

#include "gui.hpp"
#include "particle.hpp"

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include <iostream>

GUI::GUI(int WIDTH, int HEIGHT)
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

  glfwSetFramebufferSizeCallback(window, [](GLFWwindow *window, int width, int height) {
    std::clog << "window size changed to " << width << " " << height << std::endl;
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

RTGUI_particles::RTGUI_particles(int WIDTH, int HEIGHT) : GUI(WIDTH, HEIGHT)
{
}

RTGUI_particles::RTGUI_particles() : RTGUI_particles(800, 400)
{
}

void RTGUI_particles::set_particles(const std::vector<Particle> &_p)
{
  p = _p;
}

void RTGUI_particles::main_loop(const std::function<void()> &callback)
{
  // Please call set_particles in callback function and return the newly generated particles

  while (!glfwWindowShouldClose(window)) {
    // Do something with particles
    glClearColor(0.921f, 0.925f, 0.933f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
}

void RTGUI_particles::set_boundary()
{
}

RTGUI_particles::~RTGUI_particles() = default;
