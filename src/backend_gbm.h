

#pragma once

#include <gbm.h>
#include <EGL/egl.h>
#include "backend_drm.h"
#include "ieglpaltform.hpp"

namespace cx {

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

  auto getNativeDisplayType() -> void * override;
  auto getNativeWindowType() -> void * override;
  auto getPlatform() -> EGLenum override;
  auto getWidth() -> uint32_t override;
  auto getHeight() -> uint32_t override;
};

} // namespace cx
