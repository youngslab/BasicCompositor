
#pragma once

#include <GLES3/gl3.h>
#include <vector>
#include <EGL/egl.h>

namespace cx {

struct Plane {
  int fd;
  GLint stride;
  GLint offset;
};

struct Buffer {
  int format;
  GLint width;
  GLint height;
  std::vector<Plane> planes;
};

struct IEglPlatform {
  virtual auto getNativeDisplayType() -> void * = 0;
  virtual auto getNativeWindowType() -> void * = 0;
  virtual auto getPlatform() -> EGLenum = 0;
  virtual auto getWidth() -> uint32_t = 0;
  virtual auto getHeight() -> uint32_t = 0;
};
} // namespace cx
