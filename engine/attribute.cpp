
#include "attribute.hpp"
#include <fmt/format.h>

namespace lunar {
namespace engine {

auto convertTypeToSize(GLenum type) -> uint32_t {
  switch (type) {
  case GL_FLOAT:
    return sizeof(float);
  case GL_UNSIGNED_INT:
    return sizeof(uint32_t);
  case GL_INT:
    return sizeof(int32_t);
  case GL_SHORT:
    return sizeof(int16_t);
  case GL_UNSIGNED_SHORT:
    return sizeof(uint16_t);
  case GL_BYTE:
    return sizeof(char);
  case GL_UNSIGNED_BYTE:
    return sizeof(unsigned char);
  default:
    throw std::runtime_error(
	fmt::format("Failed to parse the type of {}", type));
  }
}

auto Attribute::size() const -> uint32_t {
  return convertTypeToSize(this->type) * this->count;
}
} // namespace engine
} // namespace lunar
