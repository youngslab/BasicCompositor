

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
  auto createBuffer(uint32_t w, uint32_t h, uint32_t f) -> Buffer;

  virtual auto getNativeDisplayType() -> void * override;
  virtual auto getNativeWindowType() -> void * override;
  virtual auto getPlatform() -> EGLenum override;
  virtual auto getWidth() -> uint32_t override;
  virtual auto getHeight() -> uint32_t override;
};

} // namespace cx
