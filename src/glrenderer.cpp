
#include "glrenderer.hpp"
#include "object/gl.hpp"
#include <GLES3/gl3.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <fmt/format.h>
#include <numeric>

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
		       void *native_window) {

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
    throw std::runtime_error(fmt::format(
	"ERROR: Failed to create a EGL context. {:#x}", eglGetError()));
  }

  // create egl sruface
  _egl_surface = egl::createPlatformWindowSurfaceExt_(_display, config,
						      native_window, nullptr);
  if (_egl_surface == EGL_NO_SURFACE) {
    throw std::runtime_error(fmt::format(
	"ERROR: Failed to create a EGL surface. {:#x}", eglGetError()));
  }
  eglMakeCurrent(_display, _egl_surface, _egl_surface, _context);
}

auto GlRenderer::clear(int width, int height) -> void {
  // glClearColor(0.0f, 1.0f, 0.0, 1.0);
  glClearColor(0.0, 0.0, 0.0, 0.5);
  glClear(GL_COLOR_BUFFER_BIT);
  glViewport(0, 0, width, height);
}

auto GlRenderer::clear(int w, int h, float r, float g, float b) -> void {
  glClearColor(r, g, b, 0.5);
  glClear(GL_COLOR_BUFFER_BIT);
  glViewport(0, 0, w, h);
}

auto GlRenderer::render(Entity const &e) -> void {
  e.bind();
  e.draw();
}

auto GlRenderer::swapBuffer() -> void {
  eglSwapBuffers(_display, _egl_surface);
}

auto GlRenderer::finish() -> void { glFinish(); };

auto GlRenderer::createTexture(Buffer const &b) -> gl::Texture {
  GLint attribs[] = {EGL_WIDTH,
		     b.width,
		     EGL_HEIGHT,
		     b.height,
		     EGL_LINUX_DRM_FOURCC_EXT,
		     b.format,
		     EGL_DMA_BUF_PLANE0_FD_EXT,
		     b.planes[0].fd,
		     EGL_DMA_BUF_PLANE0_OFFSET_EXT,
		     b.planes[0].offset,
		     EGL_DMA_BUF_PLANE0_PITCH_EXT,
		     b.planes[0].stride,
		     EGL_NONE};

  auto eglImage = egl::createImageKHR(_display, EGL_NO_CONTEXT,
				      EGL_LINUX_DMA_BUF_EXT, NULL, attribs);

  auto texture = gl::genTexture();
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  static auto glEGLImageTargetTexture2DOES =
      (PFNGLEGLIMAGETARGETTEXTURE2DOESPROC)eglGetProcAddress(
	  "glEGLImageTargetTexture2DOES");
  if (!glEGLImageTargetTexture2DOES) {
    throw std::runtime_error(
	"EGL doesn't support glEGLImageTargetTexture2DOES");
  }

  glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, eglImage);

  texture.dependOn(eglImage);
  return texture;
}

auto GlRenderer::createFramebuffer(Buffer const &b) -> gl::Framebuffer {
  GLint attribs[] = {EGL_WIDTH,
		     b.width,
		     EGL_HEIGHT,
		     b.height,
		     EGL_LINUX_DRM_FOURCC_EXT,
		     b.format,
		     EGL_DMA_BUF_PLANE0_FD_EXT,
		     b.planes[0].fd,
		     EGL_DMA_BUF_PLANE0_OFFSET_EXT,
		     b.planes[0].offset,
		     EGL_DMA_BUF_PLANE0_PITCH_EXT,
		     b.planes[0].stride,
		     EGL_NONE};

  auto eglImage = egl::createImageKHR(_display, EGL_NO_CONTEXT,
				      EGL_LINUX_DMA_BUF_EXT, NULL, attribs);

  auto texture = gl::genTexture();
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  static auto glEGLImageTargetTexture2DOES =
      (PFNGLEGLIMAGETARGETTEXTURE2DOESPROC)eglGetProcAddress(
	  "glEGLImageTargetTexture2DOES");
  if (!glEGLImageTargetTexture2DOES) {
    throw std::runtime_error(
	"EGL doesn't support glEGLImageTargetTexture2DOES");
  }

  glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, eglImage);

  auto framebuffer = gl::genFramebuffer();
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
			 texture, 0);

  auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (status != GL_FRAMEBUFFER_COMPLETE)
    throw std::runtime_error(fmt::format(
	"Failed to create a framebuffer. status={:#x}, glError={:#x}", status,
	glGetError()));

  // XXX: depends on a texture
  framebuffer.dependOn(texture);
  framebuffer.dependOn(eglImage);

  return framebuffer;
}

auto GlRenderer::renderToDefault() -> void {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

auto GlRenderer::renderTo(gl::Framebuffer const &fb) -> void {
  glBindFramebuffer(GL_FRAMEBUFFER, fb);
}

auto GlRenderer::blit(gl::Framebuffer const &src, uint32_t srcX, uint32_t srcY,
		      uint32_t srcWidth, uint32_t srcHeight, uint32_t destX,
		      uint32_t destY, uint32_t destWidth, uint32_t destHeight)
    -> void {
  glBindFramebuffer(GL_READ_FRAMEBUFFER, src);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer

  glBlitFramebuffer(srcX, srcY, srcWidth, srcHeight, destX, destY, destWidth,
		    destHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Entity::Entity(Mesh mesh, Material material,
	       std::vector<gl::Texture> const &textures)
    : _mesh(mesh), _material(material), _textures(textures) {}

auto Entity::bind() const -> void {
  _material.bind(); // Load the vertex data
  _mesh.bind();

  for (int i = 0; i < _textures.size(); i++) {
    glActiveTexture(GL_TEXTURE0 + i);
    glBindTexture(GL_TEXTURE_2D, _textures[i]);
  }
}

auto Entity::draw() const -> void { _mesh.draw(); }

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

Material::Material() : _program(0) {}

Material::Material(gl::Program p) : _program(p) {}

auto Material::bind() const -> void { glUseProgram(_program); }

} // namespace cx
