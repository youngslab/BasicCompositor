

#pragma once

#include <gbm.h>
#include <EGL/egl.h>
#include "backend_drm.h"

namespace cx {

struct IEglPlatform {
  virtual auto getNativeDisplayType() -> EGLNativeDisplayType = 0;
  virtual auto getNativeWindowType() -> EGLNativeWindowType = 0;
  virtual auto getPlatform() -> EGLenum = 0;
};

class GbmBackend : public IEglPlatform {

private:
  DrmBackend _drm;
  gbm_device *_gbmDevice;
  gbm_surface *_gbmSurface;
  gbm_bo *_gbmBuffer;

public:
  GbmBackend(char const *drmPath);

  // swap
  auto commit() -> void;

  auto getNativeDisplayType() -> EGLNativeDisplayType override;
  auto getNativeWindowType() -> EGLNativeWindowType override;
  auto getPlatform() -> EGLenum override;
};

} // namespace cx
