
#include "renderer_offscreen.h"
#include "entities.hpp"
#include <GLES3/gl3.h>

namespace cx {
// swap
OffscreenRenderer::OffscreenRenderer(GlRenderer &r, uint32_t w, uint32_t h)
    : _renderer(r), _framebuffer(gl::genFramebuffer()), _width(w), _height(h) {

  gl::bind(_framebuffer);

  auto renderbuffer = gl::genRenderbuffer();
  _framebuffer.dependOn(renderbuffer);

  gl::bind(renderbuffer);

  glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB8, w, h);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			    GL_RENDERBUFFER, renderbuffer);

  GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (status != GL_FRAMEBUFFER_COMPLETE)
    throw std::runtime_error(fmt::format(
	"Failed to create a framebuffer. status={:#x}, glError={:#x}", status,
	glGetError()));
}

auto OffscreenRenderer::clear(int width, int height) -> void {
  gl::bind(_framebuffer);

  glClearColor(0.0, 0.0, 0.0, 0.5);
  glClear(GL_COLOR_BUFFER_BIT);
  glViewport(0, 0, width, height);
}

auto OffscreenRenderer::render(Entity const &e) -> void { _renderer.render(e); }

auto OffscreenRenderer::swapBuffer() -> void {
  glFinish();
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

auto OffscreenRenderer::blit(uint32_t destX, uint32_t destY, uint32_t destWidth,
			     uint32_t destHeight) -> void {
  glBindFramebuffer(GL_READ_FRAMEBUFFER, _framebuffer);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer

  glBlitFramebuffer(0, 0, _width, _height, destX, destY, destWidth, destHeight,
		    GL_COLOR_BUFFER_BIT, GL_NEAREST);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

} // namespace cx
