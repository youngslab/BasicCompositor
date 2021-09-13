

#pragma once

#include <stdint.h>
#include <EGL/egl.h>

namespace lunar {
namespace backend {

class INative {
public:
  // Platform APIs
  virtual auto getNativeDisplayType() -> void * = 0;
  virtual auto getNativeWindowType() -> void * = 0;
  virtual auto getPlatform() -> EGLenum = 0;
  virtual auto getWidth() -> uint32_t = 0;
  virtual auto getHeight() -> uint32_t = 0;
};
} // namespace backend
} // namespace lunar
