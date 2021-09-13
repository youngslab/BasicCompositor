
#pragma once

#include <gbm.h>
#include "drm.hpp"
#include "native.hpp"
#include "backend.hpp"
#include "../common/Buffer.hpp"
#include <object/gl.hpp>

namespace lunar {
namespace backend {
namespace native {

class Surfaceless : public INative {
private:
  DrmBackend _drm;
  gbm_device *_gbmDevice;

  uint32_t _width;
  uint32_t _height;
  uint32_t _format;
  Buffer _buffer;

public:
  Surfaceless(char const *drmPath, uint32_t width, uint32_t height,
	      uint32_t format);

  virtual auto getNativeDisplayType() -> void * override;
  virtual auto getNativeWindowType() -> void * override;
  virtual auto getPlatform() -> EGLenum override;
  virtual auto getWidth() -> uint32_t override;
  virtual auto getHeight() -> uint32_t override;

  auto commit() -> void;
  auto create(uint32_t width, uint32_t height, uint32_t format) -> Buffer;
  auto getBuffer() -> Buffer;
};

} // namespace native

class Surfaceless : public native::Surfaceless, public Backend {
private:
  Buffer _buffer;
  gl::Framebuffer _fb;

public:
  Surfaceless(char const *drmPath, uint32_t width, uint32_t height,
	      uint32_t format);

  virtual ~Surfaceless();

  virtual auto swap() -> void override;
};

} // namespace backend
} // namespace lunar
