
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

  while (true) {

    renderer.clear(backend.getWidth(), backend.getHeight());

    // get buffers from object and render it to the framebuffer
    renderer.render();

    // redering all to the frambuffer
    renderer.swapBuffer();

    // buffer object
    backend.commit();
  }

  return 0;
}
