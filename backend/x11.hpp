
#pragma once

#include "backend.hpp"
#include "native.hpp"
#include "EGL/egl.h"
#include <X11/Xlib.h>

namespace lunar {
namespace backend {
namespace native {

class X11 : public INative {
protected:
  ::Display *_display;
  ::Window _window;
  EGLenum _platform;
  uint32_t _width;
  uint32_t _height;

public:
  X11(const char *appName, uint32_t width, uint32_t height);

  // Platform APIs
  virtual auto getNativeDisplayType() -> void * override;
  virtual auto getNativeWindowType() -> void * override;
  virtual auto getPlatform() -> EGLenum override;
  virtual auto getWidth() -> uint32_t override;
  virtual auto getHeight() -> uint32_t override;
};
} // namespace native

class X11 : public native::X11, public Backend {
public:
  X11(const char *appName, uint32_t width, uint32_t height);
  virtual ~X11();
};
} // namespace backend
} // namespace lunar
