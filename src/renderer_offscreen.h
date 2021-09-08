

#pragma once

#include <gbm.h>
#include <EGL/egl.h>
#include "glrenderer.hpp"

namespace cx {

class OffscreenRenderer {
private:
  GlRenderer &_renderer;
  gl::Framebuffer _framebuffer;
  uint32_t _width;
  uint32_t _height;

public:
  OffscreenRenderer(GlRenderer &r, uint32_t w, uint32_t h);

  auto clear(int width, int height) -> void;

  auto render(Entity const &e) -> void;

  auto swapBuffer() -> void;

  auto blit(uint32_t destX, uint32_t destY, uint32_t destWidth,
	    uint32_t destHeight) -> void;
};

} // namespace cx
