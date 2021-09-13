#include <GLES3/gl3.h>
#include <stdexcept>
#include <vector>
#include <set>
#include <map>
#include <functional>
#include <iostream>
#include <exception>

// open
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <fmt/format.h>

#include "../engine/lunar.hpp"

#include "../backend/offscreen.hpp"

#ifdef BACKEND_X11
#include "../backend/x11.hpp"
#endif

#ifdef BACKEND_WAYLAND
#include "../backend/wayland.hpp"
#endif

#ifdef BACKEND_GBM
#include "../backend/gbm.hpp"
#endif

auto createEntity() -> lunar::engine::Entity;
auto createEntity(gl::Texture const &x) -> lunar::engine::Entity;

int main(int argc, char **argv) {

#ifdef BACKEND_X11
  auto backend = lunar::backend::X11("lunar", 600, 600);
  // auto gbm = cx::GbmBackend("/dev/dri/card0");
#else
  // auto backend = lunar::backend::Wayland(nullptr, "lunar", 600, 600);
  auto backend = lunar::backend::Gbm("/dev/dri/card0");
#endif

  auto surfaceless = lunar::backend::Surfaceless("/dev/dri/card0", 300, 300,
						 DRM_FORMAT_ARGB8888);

  // auto renderer = cx::GlRenderer(platform, nativeDisplay, nativeWindow);
  auto entity = createEntity();

  //// create dmabuf
  // auto buff = gbm.createBuffer(500, 500, GBM_FORMAT_ARGB8888);

  //// create framebuffer redering to
  // auto buffFamebuffer = renderer.createFramebuffer(buff);

  //// create entity to be rendered
  // auto buffTexture = renderer.createTexture(buff);
  // auto buffEntity = createEntity(buffTexture);
  //
  surfaceless.bind();
  auto buffer = surfaceless.getBuffer();

  backend.bind();
  auto texutre = backend.createTexture(buffer);
  auto fbEntity = createEntity(texutre);

  while (true) {

    surfaceless.bind();
    glClearColor(1.0f, 1.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, 300, 300);

    lunar::engine::render(entity);

    surfaceless.swap();

    backend.bind();

    glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, 600, 600);

    lunar::engine::render(fbEntity);

    backend.swap();
  }

  return 0;
}

auto createEntity() -> lunar::engine::Entity {
  std::vector<float> vertices = {
      // positions          // colors           // texture coords
      0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
      0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
      -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
      -0.5f, 0.5f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f	// top left
  };

  std::vector<uint32_t> indices = {
      // note that we start from 0!
      0, 1, 3, // first Triangle
      1, 2, 3  // second Triangle
  };

  auto mesh = lunar::engine::Mesh(vertices, indices,
				  {{GL_FLOAT, 3, GL_FALSE},
				   {GL_FLOAT, 3, GL_FALSE},
				   {GL_FLOAT, 2, GL_FALSE}});

  const char *vShaderStr = "#version 300 es\n"
			   "layout(location = 0) in vec3 aPos;\n"
			   "layout(location = 1) in vec3 aColor;\n"
			   "layout(location = 2) in vec2 aTexCoord;\n"
			   "out vec3 ourColor;\n"
			   "out vec2 TexCoord;\n"
			   "void main() {\n"
			   "gl_Position = vec4(aPos, 1.0);\n"
			   "ourColor = aColor;\n"
			   "TexCoord = aTexCoord;\n"
			   "}\n";

  const char *fShaderStr = "#version 300 es\n"
			   "precision mediump float;\n"
			   "out vec4 FragColor;\n"
			   "in vec3 ourColor;\n"
			   "in vec2 TexCoord;\n"
			   "uniform sampler2D texture1;\n"
			   "uniform sampler2D texture2;\n"
			   "void\n"
			   "main() {\n"
			   "FragColor = mix(texture(texture1, TexCoord), "
			   "texture(texture2, TexCoord), 0.2);\n"
			   "}\n";

  // TODO: how to set texture's properties(mag, repeat, etc)

  auto p = gl::createProgram_(vShaderStr, fShaderStr);

  glUseProgram(p);
  glUniform1i(glGetUniformLocation(p, "texture1"), 0);
  glUniform1i(glGetUniformLocation(p, "texture2"), 1);

  auto material = lunar::engine::Material(p);

  auto t1 = gl::genTexture_("../res/container.jpg");
  auto t2 = gl::genTexture_("../res/awesomeface.png");

  return lunar::engine::Entity(mesh, material, {t1, t2});
}

auto createEntity(gl::Texture const &x) -> lunar::engine::Entity {

  std::vector<float> vertices = {
      // positions             // texture coords
      0.5f,  0.5f,  0.0f, 1.0f, 1.0f, // top right
      0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, // bottom right
      -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom left
      -0.5f, 0.5f,  0.0f, 0.0f, 1.0f  // top left
  };

  std::vector<uint32_t> indices = {
      // note that we start from 0!
      0, 1, 3, // first Triangle
      1, 2, 3  // second Triangle
  };

  auto mesh = lunar::engine::Mesh(
      vertices, indices, {{GL_FLOAT, 3, GL_FALSE}, {GL_FLOAT, 2, GL_FALSE}});

  const char *vShaderStr = "#version 300 es\n"
			   "layout(location = 0) in vec3 aPos;\n"
			   "layout(location = 1) in vec2 aTexCoord;\n"
			   "out vec2 TexCoord;\n"
			   "void main() {\n"
			   "gl_Position = vec4(aPos, 1.0);\n"
			   "TexCoord = aTexCoord;\n"
			   "}\n";

  const char *fShaderStr = "#version 300 es\n"
			   "precision mediump float;\n"
			   "out vec4 FragColor;\n"
			   "in vec2 TexCoord;\n"
			   "uniform sampler2D tex;\n"
			   "void main() {\n"
			   "FragColor = texture(tex, TexCoord);\n "
			   "}\n";

  auto p = gl::createProgram_(vShaderStr, fShaderStr);
  glUseProgram(p);
  glUniform1i(glGetUniformLocation(p, "tex"), 0);

  auto material = lunar::engine::Material(p);

  return lunar::engine::Entity(mesh, material, {x});
}

