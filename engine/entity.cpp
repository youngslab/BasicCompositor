

#include "entity.hpp"

namespace lunar {
namespace engine {

Entity::Entity(Mesh mesh, Material material,
	       std::vector<gl::Texture> const &textures)
    : _mesh(mesh), _material(material), _textures(textures) {}

auto Entity::draw() const -> void { _mesh.draw(); }

auto Entity::bind() const -> void {
  _material.bind(); // Load the vertex data
  _mesh.bind();

  for (int i = 0; i < _textures.size(); i++) {
    glActiveTexture(GL_TEXTURE0 + i);
    glBindTexture(GL_TEXTURE_2D, _textures[i]);
  }
}
} // namespace engine
} // namespace lunar
