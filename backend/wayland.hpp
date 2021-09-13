

#pragma once

#include "backend.hpp"
#include "native.hpp"
#include "EGL/egl.h"

// Platform Headers
#include <wayland-client.h>
#include <wayland-egl.h>
#include "wayland-xdg-shell-client-protocol.h"

namespace lunar {
namespace backend {
namespace native {

class Wayland : public INative {
protected:
  // Globals
  wl_display *_nativeDisplay;
  wl_registry *_registry;
  wl_shm *_shm;
  wl_compositor *_compositor;
  xdg_wm_base *_xdgWmBase;

  // Objet
  wl_egl_window *_nativeWindow;
  wl_surface *_surface;
  xdg_surface *_xdgSurface;
  xdg_toplevel *_xdgToplevel;

  // internal states
  bool _waitForConfigure;

  EGLenum _platform;
  uint32_t _width;
  uint32_t _height;

public:
  Wayland(const char *display, const char *appName, uint32_t width,
	  uint32_t height);

  ~Wayland();

  static void registry_global(void *data, struct wl_registry *wl_registry,
			      uint32_t name, const char *interface,
			      uint32_t version);

  static void xdg_surface_configure(void *data, struct xdg_surface *xdg_surface,
				    uint32_t serial);

  auto commit() -> void;

  // Platform APIs
  virtual auto getNativeDisplayType() -> void * override;
  virtual auto getNativeWindowType() -> void * override;
  virtual auto getPlatform() -> EGLenum override;
  virtual auto getWidth() -> uint32_t override;
  virtual auto getHeight() -> uint32_t override;
};
} // namespace native

class Wayland : public native::Wayland, public Backend {
public:
  Wayland(const char *display, const char *appName, uint32_t width,
	  uint32_t height);

  virtual ~Wayland();

  virtual auto swap() -> void override;
};
} // namespace backend
} // namespace lunar
