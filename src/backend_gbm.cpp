

#include "backend_gbm.h"

namespace cx {

GbmBackend::GbmBackend(char const *drmPath) : _drm(drmPath) {

  auto drmDevice = _drm.getDrmDevice();

  _gbmDevice = gbm_create_device(drmDevice);
  if (!_gbmDevice) {
    throw std::runtime_error(
	fmt::format("Failed to create gbm device. fd={}", drmDevice));
  }

  _gbmSurface = gbm_surface_create(_gbmDevice, _drm.getWidth(), _drm.getHeight(),
				   _drm.getFormat(),
				   GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING);
  if (!_gbmSurface) {
    throw std::runtime_error(fmt::format(
	"Failed to create gbm surface. device={}, hdisplay={}, vdisplay={}",
	(void *)_gbmDevice, _drm.getWidth(), _drm.getHeight()));
  }
}

auto GbmBackend::getNativeDisplayType() -> EGLNativeDisplayType {
  return (EGLNativeDisplayType)this->_gbmDevice;
}

auto GbmBackend::getNativeWindowType() -> EGLNativeWindowType {
  return (EGLNativeWindowType)this->_gbmSurface;
}

auto GbmBackend::commit() -> void {

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
} // namespace cx
