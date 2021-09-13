
#include "operations.hpp"
#include <EGL/eglext.h>
#include <GLES2/gl2ext.h>

namespace lunar {
namespace engine {

auto render(Entity const &e) -> void {
  e.bind();
  e.draw();
}

auto blit(gl::Framebuffer const &src, uint32_t srcX, uint32_t srcY,
	  uint32_t srcWidth, uint32_t srcHeight, uint32_t destX, uint32_t destY,
	  uint32_t destWidth, uint32_t destHeight) -> void {
  glBindFramebuffer(GL_READ_FRAMEBUFFER, src);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer

  glBlitFramebuffer(srcX, srcY, srcWidth, srcHeight, destX, destY, destWidth,
		    destHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

} // namespace engine
} // namespace lunar
