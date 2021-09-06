
#pragma once

#include <EGL/egl.h>

namespace cx {

struct IEglPlatform {
  virtual auto getNativeDisplayType() -> void * = 0;
  virtual auto getNativeWindowType() -> void * = 0;
  virtual auto getPlatform() -> EGLenum = 0;
  virtual auto getWidth() -> uint32_t = 0;
  virtual auto getHeight() -> uint32_t = 0;
};
} // namespace cx
