
#pragma once

#include <EGL/eglplatform.h>
#include <stdint.h>
#include <vector>
#include <odrm/odrm.hpp>

namespace lunar {
namespace backend {

struct DrmDisplay {
  od::ConnectorPtr connector;
  od::ModeInfoPtr mode;
  od::CrtcPtr crtc;
};

class DrmBackend {
private:
  /* drm device (fd)*/
  int64_t _drmDevice;

  /* frame buffer object */
  uint32_t _framebuffer;

  // group of connector, crtc, mode
  std::vector<DrmDisplay> _displays;

public:
  DrmBackend(char const *drmPath);

  virtual ~DrmBackend();

  auto getDrmDevice() -> int32_t;
  auto getWidth() -> int32_t;
  auto getHeight() -> int32_t;
  auto getFormat() -> uint32_t;
  auto commit(uint32_t handle, uint32_t pitch) -> void;
};

} // namespace backend
} // namespace lunar
