

#pragma once

#include <object/egl.hpp>
#include <object/gl.hpp>
#include "../common/Buffer.hpp"

namespace lunar {
namespace backend {

// Backend class abstract all egl operation
class Backend {
private:
  egl::Display _eglDisplay;
  egl::Context _eglContext;
  egl::Surface _eglSurface;

public:
  Backend(EGLenum platform, void *native_display, void *native_window);

  virtual ~Backend();

  virtual auto bind() -> void;

  // swap the buffers
  virtual auto swap() -> void;

  virtual auto createFramebuffer(Buffer const &b) -> gl::Framebuffer;

  virtual auto createTexture(Buffer const &b) -> gl::Texture;
};

} // namespace backend
} // namespace lunar
