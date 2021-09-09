
#include "backend_x11.hpp"
#include <EGL/eglext.h>
#include <stdexcept>

namespace cx {

X11Backend::X11Backend(uint32_t width, uint32_t height)
    : _width(width), _height(height) {
  _display = XOpenDisplay(nullptr);
	if(!_display)
		throw std::runtime_error("Failed to create X11 Backend");

  auto screen = DefaultScreen(_display);

  _window = XCreateSimpleWindow(
      _display, RootWindow(_display, screen), 10, 10, _width, _height, 1,
      BlackPixel(_display, screen), WhitePixel(_display, screen));

  XStoreName(_display, _window, "test");
  XMapWindow(_display, _window);
}

auto X11Backend::getNativeDisplayType() -> void * { return _display; }
auto X11Backend::getNativeWindowType() -> void * { return (void *)_window; }
auto X11Backend::getPlatform() -> EGLenum { return EGL_PLATFORM_X11_KHR; }
auto X11Backend::getWidth() -> uint32_t { return _width; }
auto X11Backend::getHeight() -> uint32_t { return _height; }
auto X11Backend::commit() -> void {}

} // namespace cx
