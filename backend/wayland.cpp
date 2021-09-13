

#include "wayland.hpp"

namespace lunar {
namespace backend {
namespace native {

void Wayland::xdg_surface_configure(void *data, struct xdg_surface *xdg_surface,
				    uint32_t serial) {
  auto backend = (Wayland *)data;
  xdg_surface_ack_configure(xdg_surface, serial);
  backend->_waitForConfigure = false;
}

static const struct xdg_surface_listener xdg_surface_listener = {
    .configure = Wayland::xdg_surface_configure,
};

static void xdg_wm_base_ping(void *data, struct xdg_wm_base *xdg_wm_base,
			     uint32_t serial) {
  xdg_wm_base_pong(xdg_wm_base, serial);
}

static const struct xdg_wm_base_listener xdg_wm_base_listener = {
    .ping = xdg_wm_base_ping,
};

auto Wayland::registry_global(void *data, struct wl_registry *wl_registry,
			      uint32_t name, const char *interface,
			      uint32_t version) -> void {
  auto waylandBackend = (Wayland *)data;
  if (strcmp(interface, wl_shm_interface.name) == 0) {
    waylandBackend->_shm =
	(wl_shm *)wl_registry_bind(wl_registry, name, &wl_shm_interface, 1);
  } else if (strcmp(interface, wl_compositor_interface.name) == 0) {
    waylandBackend->_compositor = (wl_compositor *)wl_registry_bind(
	wl_registry, name, &wl_compositor_interface, 4);
  } else if (strcmp(interface, xdg_wm_base_interface.name) == 0) {
    waylandBackend->_xdgWmBase = (xdg_wm_base *)wl_registry_bind(
	wl_registry, name, &xdg_wm_base_interface, 1);
    xdg_wm_base_add_listener(waylandBackend->_xdgWmBase, &xdg_wm_base_listener,
			     data);
  }
}

static void registry_global_remove(void *data, struct wl_registry *wl_registry,
				   uint32_t name) {
  /* This space deliberately left blank */
}

static const struct wl_registry_listener wl_registry_listener = {
    .global = Wayland::registry_global,
    .global_remove = registry_global_remove,
};

Wayland::Wayland(const char *display, const char *appName, uint32_t width,
		 uint32_t height)
    : _width(width), _height(height) {

  _nativeDisplay = wl_display_connect(NULL);
  if (_nativeDisplay == nullptr) {
    throw std::runtime_error("Failed to connect wl_display.");
  }

  // make registry
  _registry = wl_display_get_registry(_nativeDisplay);
  if (!_registry) {
    throw std::runtime_error("Failed to get wl_registry.");
  }
  wl_registry_add_listener(_registry, &wl_registry_listener, this);

  // sync
  wl_display_roundtrip(_nativeDisplay);

  // create surface
  _surface = wl_compositor_create_surface(_compositor);

  // native window
  _nativeWindow = wl_egl_window_create(_surface, width, height);

  // xdg surface
  _xdgSurface = xdg_wm_base_get_xdg_surface(_xdgWmBase, _surface);
  xdg_surface_add_listener(_xdgSurface, &xdg_surface_listener, this);

  // top level
  _xdgToplevel = xdg_surface_get_toplevel(_xdgSurface);
  xdg_toplevel_set_title(_xdgToplevel, appName);

  _waitForConfigure = true;

  // commit surface
  wl_surface_commit(_surface);
}

Wayland::~Wayland() {

  if (_nativeWindow)
    wl_egl_window_destroy(_nativeWindow);

  if (_xdgToplevel)
    xdg_toplevel_destroy(_xdgToplevel);

  if (_xdgSurface)
    xdg_surface_destroy(_xdgSurface);

  wl_surface_destroy(_surface);

  if (_xdgWmBase)
    xdg_wm_base_destroy(_xdgWmBase);

  if (_compositor)
    wl_compositor_destroy(_compositor);

  if (_shm)
    wl_shm_destroy(_shm);

  wl_registry_destroy(_registry);
  wl_display_flush(_nativeDisplay);
  wl_display_disconnect(_nativeDisplay);
}

auto Wayland::getNativeDisplayType() -> void * {
  return (void *)this->_nativeDisplay;
}
auto Wayland::getNativeWindowType() -> void * {
  return (void *)this->_nativeWindow;
}

auto Wayland::getPlatform() -> EGLenum { return EGL_PLATFORM_WAYLAND_KHR; }

auto Wayland::commit() -> void {
  if (_waitForConfigure) {
    wl_display_dispatch(_nativeDisplay);
  } else {
    wl_display_dispatch_pending(_nativeDisplay);
  }
}

auto Wayland::getHeight() -> uint32_t { return this->_height; }

auto Wayland::getWidth() -> uint32_t { return this->_width; }

} // namespace native

Wayland::Wayland(const char *display, const char *appName, uint32_t width,
		 uint32_t height)
    : native::Wayland(display, appName, width, height),
      Backend(getPlatform(), _nativeDisplay, _nativeWindow) {}

Wayland::~Wayland() {}

auto Wayland::swap() -> void {
  Backend::swap();
  commit();
}

} // namespace backend
} // namespace lunar

