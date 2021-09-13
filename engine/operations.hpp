

#pragma once

#include "entity.hpp"
#include <object/egl.hpp>
#include "../common/Buffer.hpp"

namespace lunar {
namespace engine {

auto render(Entity const &e) -> void;

auto blit(gl::Framebuffer const &src, uint32_t srcX, uint32_t srcY,
	  uint32_t srcWidth, uint32_t srcHeight, uint32_t destX, uint32_t destY,
	  uint32_t destWidth, uint32_t destHeight) -> void;

} // namespace engine
} // namespace lunar
