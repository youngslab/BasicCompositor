

#include "offscreen.hpp"

// XXX: Test
#include <iostream>
#include <fmt/format.h>

namespace lunar {
namespace backend {
namespace native {

Surfaceless::Surfaceless(char const *drmPath, uint32_t width, uint32_t height,
			 uint32_t format)
    : _drm(drmPath), _width(width), _height(height), _format(format) {

  auto drmDevice = _drm.getDrmDevice();

  _gbmDevice = gbm_create_device(drmDevice);
  if (!_gbmDevice) {
    throw std::runtime_error(
	fmt::format("Failed to create gbm device. fd={}", drmDevice));
  }

  _buffer = create(width, height, format);
  std::cout << fmt::format("Get buffer. width={} ,height={}", _buffer.width,
			   _buffer.height)
	    << std::endl;
}

auto Surfaceless::getNativeDisplayType() -> void * {
  return (void *)this->_gbmDevice;
}

auto Surfaceless::getNativeWindowType() -> void * {
  return (void *)EGL_NO_SURFACE;
}

auto Surfaceless::create(uint32_t width, uint32_t height, uint32_t format)
    -> Buffer {

  auto bo =
      gbm_bo_create(_gbmDevice, width, height, format, GBM_BO_USE_RENDERING);
  if (!bo) {
    throw std::runtime_error(
	fmt::format("Failed to create gbm buffer object. w={}, h={}, f={}, "
		    "errno={}",
		    width, height, format, strerror(errno)));
  }

  auto buffer = Buffer();
  buffer.width = width;
  buffer.height = height;
  buffer.format = format;
  buffer.planes.push_back(
      Plane{gbm_bo_get_fd(bo), static_cast<GLint>(gbm_bo_get_stride(bo)), 0});

  return buffer;
}

auto Surfaceless::commit() -> void {}

auto Surfaceless::getPlatform() -> EGLenum { return EGL_PLATFORM_GBM_KHR; }

auto Surfaceless::getWidth() -> uint32_t { return _width; }

auto Surfaceless::getHeight() -> uint32_t { return _height; }

auto Surfaceless::getBuffer() -> Buffer { return _buffer; }

} // namespace native

Surfaceless::Surfaceless(char const *drmPath, uint32_t width, uint32_t height,
			 uint32_t format)
    : native::Surfaceless(drmPath, width, height, format),
      Backend(getPlatform(), getNativeDisplayType(), getNativeWindowType()),
      _fb(createFramebuffer(getBuffer())) {}

Surfaceless::~Surfaceless() {}

auto Surfaceless::swap() -> void {
  Backend::swap();
  commit();
}
} // namespace backend
} // namespace lunar
