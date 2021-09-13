
#pragma once

#include <GLES2/gl2.h>

namespace lunar {
namespace engine {

struct Attribute {
  GLenum type;
  GLuint count;
  GLboolean normalized;

  auto size() const -> uint32_t;
};
} // namespace engine
} // namespace lunar
