
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

#include "glrenderer.h"
#include "backend_drm.h"

int main(int argc, char **argv) {

  auto backend = cx::DrmBackend("/dev/dri/card0");

  auto nativeDisplay = backend.getNativeDisplayType();
  auto nativeWindow = backend.getNativeWindowType();

  auto renderer =
      cx::GlRenderer(EGL_PLATFORM_GBM_KHR, nativeDisplay, nativeWindow);

  while (true) {

    renderer.clear();

    // get buffers from object and render it to the framebuffer
    renderer.draw();

    // redering all to the frambuffer
    renderer.swapBuffer();

    // buffer object
    backend.commit();
  }

  return 0;
}
