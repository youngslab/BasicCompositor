
#include "x11.hpp"
#include <EGL/eglext.h>

namespace lunar {
namespace backend {
namespace native {

X11::X11(const char *appName, uint32_t width, uint32_t height)
    : _width(width), _height(height), _platform(EGL_PLATFORM_X11_KHR) {
  _display = XOpenDisplay(nullptr);
  if (!_display)
    throw std::runtime_error("Failed to create X11 Display");

  auto screen = DefaultScreen(_display);

  _window = XCreateSimpleWindow(
      _display, RootWindow(_display, screen), 10, 10, _width, _height, 1,
      BlackPixel(_display, screen), WhitePixel(_display, screen));
  if (!_window)
    throw std::runtime_error("Failed to create X11 Window");

  XStoreName(_display, _window, appName);
  XMapWindow(_display, _window);
}

// Platform APIs
auto X11::getNativeDisplayType() -> void * { return _display; }

auto X11::getNativeWindowType() -> void * { return (void *)_window; }

auto X11::getPlatform() -> EGLenum { return _platform; }

auto X11::getWidth() -> uint32_t { return _width; }

auto X11::getHeight() -> uint32_t { return _height; }

} // namespace native

X11::X11(const char *appName, uint32_t width, uint32_t height)
    : native::X11(appName, width, height),
      Backend(getPlatform(), getNativeDisplayType(), getNativeWindowType()) {}

X11::~X11() {}

}; // namespace backend
} // namespace lunar
