
#include "material.hpp"

namespace lunar {
namespace engine {

Material::Material() : _program(0) {}

Material::Material(gl::Program p) : _program(p) {}

auto Material::bind() const -> void { glUseProgram(_program); }

} // namespace engine
} // namespace lunar
