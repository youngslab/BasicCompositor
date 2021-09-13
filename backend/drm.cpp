
#include "drm.hpp"

#include <drm/drm.h>
#include <fmt/core.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <map>
#include <xf86drm.h>

namespace lunar {
namespace backend {

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

  auto resources = od::getResources(_drmDevice);
  auto connectors = od::getConnectors(_drmDevice, resources);
  auto crtcs = od::getCrtcs(_drmDevice, resources);

  // Create a crtc mapping table.
  auto table = std::map<uint32_t, bool>{};
  std::for_each(crtcs.begin(), crtcs.end(), [&table](auto const &crtc) {
    table.insert(std::make_pair(crtc->crtc_id, false));
  });

  // build displays for each connector & crtc
  for (auto &connector : connectors) {
    auto modes = od::connectorGetModes(connector);
    auto mode = od::chooseBestMode(modes);
    auto crtc = od::chooseCrtc(_drmDevice, connector, table);
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

auto DrmBackend::getFormat() -> uint32_t { return DRM_FORMAT_ARGB8888; }

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
} // namespace backend
} // namespace lunar

