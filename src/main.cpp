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

#include <gbm.h>

#include "glrenderer.hpp"
#include "backend_gbm.h"
#include "backend_wayland.hpp"

#ifdef BACKEND_X11
#include "backend_x11.hpp"
#endif

auto createEntity() -> cx::Entity;
auto createEntity(gl::Texture const &x) -> cx::Entity;

int main(int argc, char **argv) {

#ifdef BACKEND_X11
  auto backend = cx::X11Backend(600, 600);
  auto gbm = cx::GbmBackend("/dev/dri/card0");
#else
  auto backend = cx::GbmBackend("/dev/dri/card0");
  auto &gbm = backend;
#endif
  auto nativeDisplay = backend.getNativeDisplayType();
  auto nativeWindow = backend.getNativeWindowType();
  auto platform = backend.getPlatform();

  auto renderer = cx::GlRenderer(platform, nativeDisplay, nativeWindow);
  auto entity = createEntity();

  // create dmabuf
  auto buff = gbm.createBuffer(500, 500, GBM_FORMAT_ARGB8888);

  // create framebuffer redering to
  auto buffFamebuffer = renderer.createFramebuffer(buff);

  // create entity to be rendered
  auto buffTexture = renderer.createTexture(buff);
  auto buffEntity = createEntity(buffTexture);

  while (true) {
    // renderpass 1
    renderer.renderTo(buffFamebuffer);
    renderer.clear(500, 500, 1.0f, 1.0f, 0.0f);
    renderer.render(entity);
    renderer.finish();

    // renderpass 2
    renderer.renderToDefault();
    renderer.clear(backend.getWidth(), backend.getHeight(), 1.0f, 0.0f, 0.0f);
    renderer.render(buffEntity);
    renderer.swapBuffer();
    backend.commit();
  }

  return 0;
}

auto createEntity() -> cx::Entity {
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

  auto mesh = cx::Mesh(vertices, indices,
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

  auto t1 = gl::genTexture_("../res/container.jpg");
  auto t2 = gl::genTexture_("../res/awesomeface.png");

  // TODO: how to set texture's properties(mag, repeat, etc)

  auto p = gl::createProgram_(vShaderStr, fShaderStr);

  glUseProgram(p);
  glUniform1i(glGetUniformLocation(p, "texture1"), 0);
  glUniform1i(glGetUniformLocation(p, "texture2"), 1);

  auto material = cx::Material(p);

  return cx::Entity(mesh, material, {t1, t2});
}

auto createEntity(gl::Texture const &x) -> cx::Entity {

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

  auto mesh = cx::Mesh(vertices, indices,
		       {{GL_FLOAT, 3, GL_FALSE}, {GL_FLOAT, 2, GL_FALSE}});

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

  auto material = cx::Material(p);

  return cx::Entity(mesh, material, {x});
}

