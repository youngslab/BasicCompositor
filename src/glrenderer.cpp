
#include "glrenderer.hpp"
#include "object/gl.hpp"
#include <GLES3/gl3.h>
#include <fmt/format.h>

namespace cx {

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
    ;
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

GlRenderer::GlRenderer(EGLenum platform, void *native_display,
		       void *native_window)
    : _program(0) {

  _display = egl::getPlatformDisplay(platform, native_display, nullptr);
  if (_display == EGL_NO_DISPLAY) {
    throw std::runtime_error("Failed to get platform display.");
  }

  if (!eglInitialize(_display, nullptr, nullptr))
    throw std::runtime_error("Failed to initailize EGL");

  // create an OpenGL context
  if (!eglBindAPI(EGL_OPENGL_ES_API))
    throw std::runtime_error("Failed to Bind API");

  // config
  EGLint config_attribs[] = {EGL_SURFACE_TYPE,
			     EGL_WINDOW_BIT,
			     EGL_RED_SIZE,
			     1,
			     EGL_GREEN_SIZE,
			     1,
			     EGL_BLUE_SIZE,
			     1,
			     EGL_ALPHA_SIZE,
			     1,
			     EGL_RENDERABLE_TYPE,
			     EGL_OPENGL_ES2_BIT,
			     EGL_NONE};

  EGLConfig config;
  EGLint num_config;
  eglChooseConfig(_display, config_attribs, &config, 1, &num_config);

  // create context
  static const EGLint context_attribs[] = {EGL_CONTEXT_CLIENT_VERSION, 2,
					   EGL_NONE};
  _context =
      egl::createContext(_display, config, EGL_NO_CONTEXT, context_attribs);
  if (_context == EGL_NO_CONTEXT) {
    throw std::runtime_error("Failed to create a EGL context");
  }

  // create egl sruface
  _egl_surface = egl::createWindowSurface_(
      _display, config, (EGLNativeWindowType)native_window, nullptr);
  if (_egl_surface == EGL_NO_SURFACE) {
    throw std::runtime_error("Failed to create a EGL surface");
  }
  eglMakeCurrent(_display, _egl_surface, _egl_surface, _context);
}

auto GlRenderer::clear(int width, int height) -> void {
  // glClearColor(0.0f, 1.0f, 0.0, 1.0);
  glClearColor(0.0, 0.0, 0.0, 0.5);
  glClear(GL_COLOR_BUFFER_BIT);
  glViewport(0, 0, width, height);
}

auto GlRenderer::render(Entity const &e) -> void {
  e.bind();
  e.draw();
}

auto GlRenderer::swapBuffer() -> void {
  eglSwapBuffers(_display, _egl_surface);
}

Entity::Entity(Mesh mesh, Program program) : _mesh(mesh), _program(program) {}

auto Entity::bind() const -> void {
  _program.bind(); // Load the vertex data
  _mesh.bind();
}

auto Entity::draw() const -> void { _mesh.draw(); }

Mesh::Mesh()
    : _vbo(gl::GenBuffer()), _ebo(gl::GenBuffer()), _vao(gl::GenVertexArray()) {
}

Mesh::Mesh(std::vector<float> const &vertices,
	   std::vector<uint32_t> const &indices,
	   std::vector<Attribute> const &attrs)
    : _vbo(gl::GenBuffer()), _ebo(gl::GenBuffer()), _vao(gl::GenVertexArray()) {

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
  for (int i = 0; i < attrs.size(); i++) {
    glVertexAttribPointer(i, attrs[i].count, attrs[i].type, attrs[i].normalized,
			  attrs[i].size(), (void *)offset);
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

Program::Program() : _program(0) {}

Program::Program(std::string const &v, std::string const &f)
    : _program(gl::createProgram()) {
  auto vinterm = v.c_str();
  auto vert = gl::createShader_(GL_VERTEX_SHADER, 1, &vinterm, nullptr);

  auto finterm = f.c_str();
  auto frag = gl::createShader_(GL_FRAGMENT_SHADER, 1, &finterm, nullptr);

  _program = gl::createProgram_(vert, frag);
}

auto Program::bind() const -> void { glUseProgram(_program); }

} // namespace cx
