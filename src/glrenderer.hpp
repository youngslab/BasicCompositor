
#pragma once

#include <object/egl.hpp>
#include <object/gl.hpp>

namespace cx {

struct Attribute {
  GLenum type;
  GLuint count;
  GLboolean normalized;

  auto size() const -> uint32_t;
};

class Mesh {

private:
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

class Material {
private:
  gl::Program _program;
  std::vector<gl::Texture> _textures;

public:
  Material();
  Material(gl::Program p, std::vector<gl::Texture> ts);

  auto bind() const -> void;
};

class Entity {
private:
  Material _material;
  Mesh _mesh;

public:
  Entity(Mesh mesh, Material material);

  auto draw() const -> void;
  auto bind() const -> void;
};

class GlRenderer {
private:
  egl::Display _display;
  egl::Context _context;
  egl::Surface _egl_surface;

  gl::Program _program;

  GLuint _VAO, _VBO, _EBO;

public:
  GlRenderer(EGLenum platform, void *native_display, void *native_window);

  ~GlRenderer() {}

  auto clear(int width, int height) -> void;

  auto render(Entity const &e) -> void;

  auto swapBuffer() -> void;
};

} // namespace cx
