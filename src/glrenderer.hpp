
#pragma once

#include <object/egl.hpp>
#include <object/gl.hpp>

namespace cx {

class GlRenderer {
private:
  egl::Display _display;
  egl::Context _context;
  egl::Surface _egl_surface;

  gl::Program _program;

  GLuint _VAO, _VBO, _EBO;

public:
  GlRenderer(EGLenum platform, void *native_display, void *native_window);

  ~GlRenderer() {}

  auto init() -> void;

  auto clear(int width, int height ) -> void;

  auto render() -> void;

  auto swapBuffer() -> void;
};

} // namespace cx
