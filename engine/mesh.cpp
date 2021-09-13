

#include "mesh.hpp"
#include <numeric>

namespace lunar {
namespace engine {

Mesh::Mesh()
    : _vbo(gl::genBuffer()), _ebo(gl::genBuffer()), _vao(gl::genVertexArray()) {
}

Mesh::Mesh(std::vector<float> const &vertices,
	   std::vector<uint32_t> const &indices,
	   std::vector<Attribute> const &attrs)
    : _vbo(gl::genBuffer()), _ebo(gl::genBuffer()), _vao(gl::genVertexArray()) {

  // vao
  glBindVertexArray(_vao);

  // vbo
  glBindBuffer(GL_ARRAY_BUFFER, _vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(),
	       vertices.data(), GL_STATIC_DRAW);

  // ebo
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * indices.size(),
	       indices.data(), GL_STATIC_DRAW);

  _count = indices.size();

  // records attributes
  auto offset = 0ul;
  auto stride = std::accumulate(
      attrs.begin(), attrs.end(), (uint32_t)0,
      [](uint32_t s, Attribute const &attr) { return s + attr.size(); });

  for (int i = 0; i < attrs.size(); i++) {
    glVertexAttribPointer(i, attrs[i].count, attrs[i].type, attrs[i].normalized,
			  stride, (void *)offset);
    glEnableVertexAttribArray(i);
    offset += attrs[i].size();
  }

  // unbind
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}
auto Mesh::draw() const -> void {
  glDrawElements(GL_TRIANGLES, _count, GL_UNSIGNED_INT, 0);
}

auto Mesh::bind() const -> void {
  glBindVertexArray(_vao); // seeing as we only have a single VAO there's no
}

} // namespace engine
} // namespace lunar
