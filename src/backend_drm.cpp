
#include "backend_drm.h"

#include <drm/drm.h>
#include <fmt/core.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <map>

#include "drm.hpp"

namespace cx {

auto isCrtcAssigned(uint32_t crtc_id,
		    std::map<uint32_t, bool> const &assigned) {
  return assigned.find(crtc_id) != assigned.end();
}

auto canUse(std::map<uint32_t, bool> const &crtcTable, uint32_t crtc_id)
    -> bool {
  if (crtcTable.find(crtc_id) == crtcTable.end())
    return false;
  return !crtcTable.find(crtc_id)->second;
}

auto chooseCrtc(int fd, drm::EncoderPtr encoder,
		std::map<uint32_t, bool> const &crtcTable)
    -> std::optional<drm::CrtcPtr> {

  if (encoder->crtc_id && canUse(crtcTable, encoder->crtc_id)) {
    return drm::CrtcPtr(fd, encoder->crtc_id);
  }

  for (auto it = crtcTable.begin(); it != crtcTable.end(); it++) {
    auto idx = std::distance(crtcTable.begin(), it);
    if (!(encoder->possible_crtcs & (1 << idx)))
      continue;

    if (it->second) // it used
      continue;

    return drm::CrtcPtr(fd, it->first);
  }

  return std::nullopt;
}

auto chooseCrtc(int fd, drm::ConnectorPtr connector,
		std::map<uint32_t, bool> const &crtcTable)
    -> std::optional<drm::CrtcPtr> {
  if (!(connector->connection & DRM_MODE_CONNECTED))
    return std::nullopt;

  // Check connector has an encoder already.
  uint32_t encoder_id = connector->encoder_id;
  if (encoder_id) {
    auto encoder = drm::EncoderPtr(fd, encoder_id);
    auto crtc = chooseCrtc(fd, encoder, crtcTable);
    if (crtc)
      return crtc;
  }

  // Check all encoders
  auto encoders = drm::getEncoders(fd, connector);
  for (auto &encoder : encoders) {
    auto crtc = chooseCrtc(fd, encoder, crtcTable);
    if (crtc)
      return crtc;
  }

  return std::nullopt;
}

/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/

DrmBackend::DrmBackend(const char *drmPath)
    : _displays(0), _prevFramebuffer(0), _prevGbmBuffer(nullptr) {
  _fd = open(drmPath, DRM_RDWR);
  if (this->_fd < 0) {
    throw std::runtime_error(
	fmt::format("Failed to open a DRM device.path={}, errono={} ", drmPath,
		    strerror(errno)));
  }

  auto resources = drm::getResources(_fd);
  auto connectors = drm::getConnectors(_fd, resources);
  auto crtcs = drm::getCrtcs(_fd, resources);

  // Create a crtc mapping table.
  auto table = std::map<uint32_t, bool>{};
  std::for_each(crtcs.begin(), crtcs.end(), [&table](auto const &crtc) {
    table.insert(std::make_pair(crtc->crtc_id, false));
  });

  // build displays for each connector & crtc
  for (auto &connector : connectors) {
    auto modes = drm::connectorGetModes(connector);
    auto mode = drm::chooseBestMode(modes);
    auto crtc = chooseCrtc(_fd, connector, table);
    if (!crtc)
      continue;
    // update table
    table[(*crtc)->crtc_id] = true;

    /* TODO: create framebuffer */

    // Create & push a Display object
    // create gbm

    _displays.push_back(DrmDisplay{connector, mode, *crtc});
  }

  _gbmDevice = gbm_create_device(_fd);
  _gbmSurface = gbm_surface_create(
      _gbmDevice, _displays[0].mode->hdisplay, _displays[0].mode->vdisplay,
      GBM_BO_FORMAT_XRGB8888, GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING);
}

DrmBackend::~DrmBackend() {
  if (_gbmDevice)
    gbm_device_destroy(_gbmDevice);
}

auto DrmBackend::getNativeDisplayType() -> EGLNativeDisplayType {
  return (EGLNativeDisplayType)this->_gbmDevice;
}

auto DrmBackend::getNativeWindowType() -> EGLNativeWindowType {
  return (EGLNativeWindowType)this->_gbmSurface;
}

auto DrmBackend::commit() -> void {

  // should be called after eglSwapbuffer
  // 1. Get a buffer
  struct gbm_bo *bo = gbm_surface_lock_front_buffer(_gbmSurface);
  if (bo == nullptr) {
    throw std::runtime_error("Failed to get gbm buffer object");
  }

  uint32_t handle = gbm_bo_get_handle(bo).u32;
  uint32_t pitch = gbm_bo_get_stride(bo);
  uint32_t fb;
  auto modeInfo = _displays[0].mode;

  // 2. Add a framebuffer
  drmModeAddFB(_fd, modeInfo->hdisplay, modeInfo->vdisplay, 24, 32, pitch,
	       handle, &fb);

  // 3. Add Crtc
  drmModeSetCrtc(_fd, _displays[0].crtc->crtc_id, fb, 0, 0,
		 &_displays[0].connector->connector_id, 1, modeInfo);

  if (_prevFramebuffer) {
    drmModeRmFB(_fd, _prevFramebuffer);
  }
  // 4. Remove Previous Objects
  if (_prevGbmBuffer) {
    gbm_surface_release_buffer(_gbmSurface, _prevGbmBuffer);
  }

  _prevFramebuffer = fb;
  _prevGbmBuffer = bo;
}
} // namespace cx

