
#include "glrenderer.hpp"
#include <fmt/format.h>

namespace cx {

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

  init();
}

const char *vShaderStr = "#version 300 es\n"
			 "layout(location = 0) in vec3 aPos;\n"
			 "void main()\n"
			 "{\n"
			 "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
			 "}\0";

static const char *fShaderStr = "#version 300 es\n"
				"precision mediump float;\n"
				"out vec4 FragColor;\n"
				"void main()\n"
				"{\n"
				"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
				"}\n\0";

auto GlRenderer::init() -> void {

  auto fragShader =
      gl::createShader_(GL_FRAGMENT_SHADER, 1, &fShaderStr, nullptr);

  auto vertexShader =
      gl::createShader_(GL_VERTEX_SHADER, 1, &vShaderStr, nullptr);

  _program = gl::createProgram_(vertexShader, fragShader);

  float vertices[] = {
      0.5f,  0.5f,  0.0f, // top right
      0.5f,  -0.5f, 0.0f, // bottom right
      -0.5f, -0.5f, 0.0f, // bottom left
      -0.5f, 0.5f,  0.0f  // top left
  };
  unsigned int indices[] = {
      // note that we start from 0!
      0, 1, 3, // first Triangle
      1, 2, 3  // second Triangle
  };

  glGenVertexArrays(1, &_VAO);
  glGenBuffers(1, &_VBO);
  glGenBuffers(1, &_EBO);
  // bind the Vertex Array Object first, then bind and set vertex buffer(s), and
  // then configure vertex attributes(s).
  glBindVertexArray(_VAO);

  glBindBuffer(GL_ARRAY_BUFFER, _VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
	       GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  // note that this is allowed, the call to glVertexAttribPointer registered VBO
  // as the vertex attribute's bound vertex buffer object so afterwards we can
  // safely unbind
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // remember: do NOT unbind the EBO while a VAO is active as the bound element
  // buffer object IS stored in the VAO; keep the EBO bound.
  // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // You can unbind the VAO afterwards so other VAO calls won't accidentally
  // modify this VAO, but this rarely happens. Modifying other
  // VAOs requires a call to glBindVertexArray anyways so we generally don't
  // unbind VAOs (nor VBOs) when it's not directly necessary.
  glBindVertexArray(0);

}

auto GlRenderer::clear(int width, int height) -> void {
  // glClearColor(0.0f, 1.0f, 0.0, 1.0);
  glClearColor(0.0, 0.0, 0.0, 0.5);
  glClear(GL_COLOR_BUFFER_BIT);
  glViewport(0, 0, width, height);
}

auto GlRenderer::render() -> void {
  // Use the program object
  glUseProgram(_program); // Load the vertex data

  glBindVertexArray(_VAO); // seeing as we only have a single VAO there's no
			   // need to bind it every time, but we'll do so to
			   // keep things a bit more organized
  // glDrawArrays(GL_TRIANGLES, 0, 6);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

auto GlRenderer::swapBuffer() -> void {
  eglSwapBuffers(_display, _egl_surface);
}
} // namespace cx
