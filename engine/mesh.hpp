
#pragma once

#include <object/gl.hpp>
#include "attribute.hpp"

namespace lunar {
namespace engine {
class Mesh {

protected:
  gl::Buffer _vbo;
  gl::Buffer _ebo;
  gl::VertexArray _vao;

  uint32_t _count;

public:
  Mesh(std::vector<float> const &vertices, std::vector<uint32_t> const &indices,
       std::function<void()> descriptor);
  Mesh(std::vector<float> const &vertices, std::vector<uint32_t> const &indices,
       std::vector<Attribute> const &attr);
  Mesh();

  auto draw() const -> void;
  auto bind() const -> void;
};
} // namespace engine
} // namespace lunar
