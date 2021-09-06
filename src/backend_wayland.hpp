

#pragma once

#include <wayland-client.h>
#include <wayland-egl.h>
#include "wayland-xdg-shell-client-protocol.h"
#include "ieglpaltform.hpp"

namespace cx {
class WaylandBackend : public IEglPlatform {
private:
  // Globals
  wl_display *_display;
  wl_registry *_registry;
  wl_shm *_shm;
  wl_compositor *_compositor;
  xdg_wm_base *_xdgWmBase;

  // Objet
  wl_egl_window *_nativeWindow;
  wl_surface *_surface;
  xdg_surface *_xdgSurface;
  xdg_toplevel *_xdgToplevel;

  bool _waitForConfigure;
  uint32_t _width;
  uint32_t _height;

public:
  WaylandBackend(const char *display, uint32_t width, uint32_t height);
  ~WaylandBackend();

  static void registry_global(void *data, struct wl_registry *wl_registry,
			      uint32_t name, const char *interface,
			      uint32_t version);

  static void xdg_surface_configure(void *data, struct xdg_surface *xdg_surface,
				    uint32_t serial);

  auto commit() -> void;

  virtual auto getNativeDisplayType() -> void * override;
  virtual auto getNativeWindowType() -> void * override;
  virtual auto getPlatform() -> EGLenum override;
  virtual auto getWidth() -> uint32_t override;
  virtual auto getHeight() -> uint32_t override;
};

} // namespace cx
