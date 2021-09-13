
#pragma once

#include <GLES2/gl2.h>

#include "material.hpp"
#include "mesh.hpp"

namespace lunar {
namespace engine {

class Entity {
protected:
  Material _material;
  Mesh _mesh;
  std::vector<gl::Texture> _textures;

public:
  Entity(Mesh mesh, Material material,
	 std::vector<gl::Texture> const &textures);

  auto draw() const -> void;
  auto bind() const -> void;
};
} // namespace engine
} // namespace lunar
