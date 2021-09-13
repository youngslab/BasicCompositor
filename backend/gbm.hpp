

#pragma once

#include <gbm.h>
#include <EGL/egl.h>

#include "drm.hpp"
#include "native.hpp"
#include "backend.hpp"

namespace lunar {
namespace backend {
namespace native {

class Gbm : public INative {

private:
  DrmBackend _drm;
  gbm_device *_gbmDevice;
  gbm_surface *_gbmSurface;
  gbm_bo *_gbmBuffer;

public:
  Gbm(char const *drmPath);

  // swap
  auto commit() -> void;
  // auto createBuffer(uint32_t w, uint32_t h, uint32_t f) -> Buffer;

  virtual auto getNativeDisplayType() -> void * override;
  virtual auto getNativeWindowType() -> void * override;
  virtual auto getPlatform() -> EGLenum override;
  virtual auto getWidth() -> uint32_t override;
  virtual auto getHeight() -> uint32_t override;
};
} // namespace native

class Gbm : public native::Gbm, public Backend {
public:
  Gbm(char const *drmPath);

  virtual ~Gbm();

  virtual auto swap() -> void override;
};

} // namespace backend
} // namespace lunar
