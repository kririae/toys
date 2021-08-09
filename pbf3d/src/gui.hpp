//
// Created by kr2 on 8/9/21.
//

#ifndef PBF3D_SRC_GUI_HPP_
#define PBF3D_SRC_GUI_HPP_

#include <functional>

struct GLFWwindow;
struct Particle;

class GUI {
 public:
  GUI(int WIDTH, int HEIGHT);
  GUI();
  virtual ~GUI();
  virtual void main_loop(const std::function<void()> &callback);

 protected:
  GLFWwindow *window{};
  int width{}, height{};
};

class RTGUI_particles : public GUI {
  // REAL-TIME GUI
 public:
  RTGUI_particles(int WIDTH, int HEIGHT);
  RTGUI_particles();
  ~RTGUI_particles() override;

  void set_particles(const std::vector<Particle> &p);
  void set_boundary();
  void main_loop(const std::function<void()> &callback) override;

 protected:
  std::vector<Particle> p;
};

#endif  // PBF3D_SRC_GUI_HPP_
