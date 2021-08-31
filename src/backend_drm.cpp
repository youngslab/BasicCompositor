
#include "backend_drm.h"

#include <drm/drm.h>
#include <fmt/core.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <map>
#include <xf86drm.h>

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
    : _displays(0), _framebuffer(0), _drmDevice(-1) {

  _drmDevice = open(drmPath, DRM_RDWR | DRM_CLOEXEC);
  if (_drmDevice < 0) {
    throw std::runtime_error(
	fmt::format("Failed to open a DRM device.path={}, errono={} ", drmPath,
		    strerror(errno)));
  }

  auto resources = drm::getResources(_drmDevice);
  auto connectors = drm::getConnectors(_drmDevice, resources);
  auto crtcs = drm::getCrtcs(_drmDevice, resources);

  // Create a crtc mapping table.
  auto table = std::map<uint32_t, bool>{};
  std::for_each(crtcs.begin(), crtcs.end(), [&table](auto const &crtc) {
    table.insert(std::make_pair(crtc->crtc_id, false));
  });

  // build displays for each connector & crtc
  for (auto &connector : connectors) {
    auto modes = drm::connectorGetModes(connector);
    auto mode = drm::chooseBestMode(modes);
    auto crtc = chooseCrtc(_drmDevice, connector, table);
    if (!crtc)
      continue;
    // update table
    table[(*crtc)->crtc_id] = true;

    // Create & push a Display object
    // create gbm
    _displays.push_back(DrmDisplay{connector, mode, *crtc});
  }

  if (_displays.size() == 0) {
    throw std::runtime_error("Failed to find displays");
  }
}

DrmBackend::~DrmBackend() {
  if (_framebuffer >= 0) {
    drmModeRmFB(_drmDevice, _framebuffer);
  }
}

auto DrmBackend::getDrmDevice() -> int32_t { return this->_drmDevice; }

auto DrmBackend::getWidth() -> int32_t {
  return this->_displays[0].mode->hdisplay;
}
auto DrmBackend::getHeight() -> int32_t {
  return this->_displays[0].mode->vdisplay;
}

auto DrmBackend::getFormat() -> uint32_t { return DRM_FORMAT_XRGB8888; }

auto DrmBackend::commit(uint32_t handle, uint32_t pitch) -> void {
  auto oldFramebuffer = _framebuffer;
  auto modeInfo = _displays[0].mode;
  drmModeAddFB(_drmDevice, modeInfo->hdisplay, modeInfo->vdisplay, 24, 32,
	       pitch, handle, &_framebuffer);

  drmModeSetCrtc(_drmDevice, _displays[0].crtc->crtc_id, _framebuffer, 0, 0,
		 &_displays[0].connector->connector_id, 1, modeInfo);

  if (oldFramebuffer) {
    drmModeRmFB(_drmDevice, oldFramebuffer);
  }
}
} // namespace cx

