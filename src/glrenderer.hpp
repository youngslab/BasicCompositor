
#pragma once

#include <object/egl.hpp>
#include <object/gl.hpp>
#include "ieglpaltform.hpp"

namespace cx {

struct Attribute {
  GLenum type;
  GLuint count;
  GLboolean normalized;

  auto size() const -> uint32_t;
};

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

class Material {
protected:
  gl::Program _program;

public:
  Material();
  Material(gl::Program p);

  auto bind() const -> void;
};

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

class GlRenderer {
private:
  egl::Display _display;
  egl::Context _context;
  egl::Surface _egl_surface;

public:
  GlRenderer(EGLenum platform, void *native_display, void *native_window);

  ~GlRenderer() {}

  auto clear(int width, int height) -> void;

  auto clear(int w, int h, float r, float g, float b) -> void;

  auto render(Entity const &e) -> void;

  auto renderTo(gl::Framebuffer const &fb) -> void;
  auto renderToDefault() -> void;

  auto blit(gl::Framebuffer const &src, uint32_t srcX, uint32_t srcY,
	    uint32_t srcWidth, uint32_t srcHeight, uint32_t destX,
	    uint32_t destY, uint32_t destWidth, uint32_t destHeight) -> void;

  auto finish() -> void;

  auto swapBuffer() -> void;

  auto createFramebuffer(Buffer const &buffer) -> gl::Framebuffer;
  auto createTexture(Buffer const &buffer) -> gl::Texture;
};

// Generic APIs

template <typename Entity> inline auto render(Entity const &entity) {
  entity.render();
}

} // namespace cx
