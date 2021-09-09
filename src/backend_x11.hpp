

#pragma once

#include <X11/Xlib.h>
#include "ieglpaltform.hpp"

namespace cx {

class X11Backend : public IEglPlatform {

private:
  ::Display *_display;
  ::Window _window;
  uint32_t _width;
  uint32_t _height;

public:
  X11Backend(uint32_t width, uint32_t height);

  virtual auto getNativeDisplayType() -> void * override;
  virtual auto getNativeWindowType() -> void * override;
  virtual auto getPlatform() -> EGLenum override;
  virtual auto getWidth() -> uint32_t override;
  virtual auto getHeight() -> uint32_t override;
  auto commit() -> void;
};

} // namespace cx
