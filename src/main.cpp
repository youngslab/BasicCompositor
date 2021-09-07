
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

int main(int argc, char **argv) {

  // auto backend = cx::GbmBackend("/dev/dri/card0");
  auto backend = cx::WaylandBackend(nullptr, 500, 500);
  auto nativeDisplay = backend.getNativeDisplayType();
  auto nativeWindow = backend.getNativeWindowType();
  auto platform = backend.getPlatform();

  auto renderer = cx::GlRenderer(platform, nativeDisplay, nativeWindow);

  std::vector<float> vertices = {
      0.5f,  0.5f,  0.0f, // top right
      0.5f,  -0.5f, 0.0f, // bottom right
      -0.5f, -0.5f, 0.0f, // bottom left
      -0.5f, 0.5f,  0.0f  // top left
  };
  std::vector<uint32_t> indices = {
      // note that we start from 0!
      0, 1, 3, // first Triangle
      1, 2, 3  // second Triangle
  };

  auto mesh = cx::Mesh(vertices, indices, {{GL_FLOAT, 3, GL_FALSE}});

  const char *vShaderStr =
      "#version 300 es\n"
      "layout(location = 0) in vec3 aPos;\n"
      "void main()\n"
      "{\n"
      "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
      "}\0";

  const char *fShaderStr = "#version 300 es\n"
			   "precision mediump float;\n"
			   "out vec4 FragColor;\n"
			   "void main()\n"
			   "{\n"
			   "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
			   "}\0";

  auto program = cx::Program(vShaderStr, fShaderStr);

  auto entity = cx::Entity(mesh, program);

  while (true) {
    renderer.clear(backend.getWidth(), backend.getHeight());

    // get buffers from object and render it to the framebuffer
    renderer.render(entity);

    // redering all to the frambuffer
    renderer.swapBuffer();

    // buffer object
    backend.commit();
  }

  return 0;
}
