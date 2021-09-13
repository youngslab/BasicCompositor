

#include "gbm.hpp"
#include <EGL/eglext.h>

namespace lunar {
namespace backend {
namespace native {

Gbm::Gbm(char const *drmPath) : _drm(drmPath) {

  auto drmDevice = _drm.getDrmDevice();

  _gbmDevice = gbm_create_device(drmDevice);
  if (!_gbmDevice) {
    throw std::runtime_error(
	fmt::format("Failed to create gbm device. fd={}", drmDevice));
  }

  _gbmSurface = gbm_surface_create(_gbmDevice, _drm.getWidth(),
				   _drm.getHeight(), _drm.getFormat(),
				   GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING);
  if (!_gbmSurface) {
    throw std::runtime_error(fmt::format(
	"Failed to create gbm surface. device={}, hdisplay={}, vdisplay={}",
	(void *)_gbmDevice, _drm.getWidth(), _drm.getHeight()));
  }
}

auto Gbm::getNativeDisplayType() -> void * { return (void *)this->_gbmDevice; }

auto Gbm::getNativeWindowType() -> void * { return (void *)this->_gbmSurface; }

auto Gbm::commit() -> void {

  // should be called after eglSwapbuffer
  // 1. Get a buffer
  auto oldGbmBuffer = _gbmBuffer;
  _gbmBuffer = gbm_surface_lock_front_buffer(_gbmSurface);
  if (_gbmBuffer == nullptr) {
    throw std::runtime_error("Failed to get gbm buffer object");
  }

  uint32_t handle = gbm_bo_get_handle(_gbmBuffer).u32;
  uint32_t pitch = gbm_bo_get_stride(_gbmBuffer);

  _drm.commit(handle, pitch);

  if (oldGbmBuffer) {
    gbm_surface_release_buffer(_gbmSurface, oldGbmBuffer);
  }
}

auto Gbm::getPlatform() -> EGLenum { return EGL_PLATFORM_GBM_KHR; }

auto Gbm::getWidth() -> uint32_t { return this->_drm.getWidth(); }
auto Gbm::getHeight() -> uint32_t { return this->_drm.getHeight(); }

// auto Gbm::createBuffer(uint32_t w, uint32_t h, uint32_t f) -> Buffer {
// int oldErrno = errno;
//// TODO: How to remove buffer?
// auto bo = gbm_bo_create(_gbmDevice, w, h, f, GBM_BO_USE_RENDERING);
// if (!bo) {
// throw std::runtime_error(
// fmt::format("Failed to create gbm buffer object. w={}, h={}, f={}, "
//"errno={}, oldErrno={}",
// w, h, f, strerror(errno), strerror(oldErrno)));
//}

// auto buffer = Buffer();
// buffer.width = w;
// buffer.height = h;
// buffer.format = f;
// buffer.planes.push_back(
// Plane{gbm_bo_get_fd(bo), static_cast<GLint>(gbm_bo_get_stride(bo)), 0});
// return buffer;
//}

} // namespace native
Gbm::Gbm(char const *drmPath)
    : native::Gbm(drmPath),
      Backend(getPlatform(), getNativeDisplayType(), getNativeWindowType()) {}

Gbm::~Gbm() {}

auto Gbm::swap() -> void {
  Backend::swap();
  commit();
}
} // namespace backend
} // namespace lunar
