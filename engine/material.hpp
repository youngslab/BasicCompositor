
#pragma once

#include <object/gl.hpp>

namespace lunar {
namespace engine {

class Material {
protected:
  gl::Program _program;

public:
  Material();
  Material(gl::Program p);

  auto bind() const -> void;
};

} // namespace engine
} // namespace lunar
