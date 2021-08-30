
#pragma once

#include <EGL/eglplatform.h>
#include <gbm.h>
#include <stdint.h>
#include <vector>
#include "drm.hpp"

namespace cx {

struct IEglPlatform {
  virtual auto getNativeDisplayType() -> EGLNativeDisplayType = 0;
  virtual auto getNativeWindowType() -> EGLNativeWindowType = 0;
};

struct DrmDisplay {
  drm::ConnectorPtr connector;
  drm::ModeInfoPtr mode;
  drm::CrtcPtr crtc;
};

class DrmBackend : public IEglPlatform {
private:
  /* drm device */
  int64_t _fd;
  gbm_device *_gbmDevice;
  gbm_surface *_gbmSurface;
	gbm_bo *_prevGbmBuffer;
	uint32_t _prevFramebuffer;

  // use only one display
  std::vector<DrmDisplay> _displays;

public:
  DrmBackend(char const *drmPath);

  virtual ~DrmBackend();

  auto getNativeDisplayType() -> EGLNativeDisplayType override;
  auto getNativeWindowType() -> EGLNativeWindowType override;

  auto commit() -> void;
};


} // namespace cx
