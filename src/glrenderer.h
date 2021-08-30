
#pragma once

#include <EGL/egl.h>
#include <EGL/eglext.h>

#include <EGL/eglplatform.h>
#include <GLES3/gl3.h>

namespace cx {

class GlRenderer {
private:
  EGLDisplay _display;
  EGLContext _context;
  EGLSurface _egl_surface;

public:
  GlRenderer(EGLenum platform, EGLNativeDisplayType native_display,
	     EGLNativeWindowType native_window) {
    // gbm_device = gbm_create_device(device);
    //_display = eglGetPlatformDisplay(platform, native_display, nullptr);
    _display = eglGetDisplay(native_display);
    if (_display == EGL_NO_DISPLAY) {
      throw std::runtime_error("Failed to get platform display.");
    }

    eglInitialize(_display, nullptr, nullptr);

    // create an OpenGL context
    eglBindAPI(EGL_OPENGL_API);

    // config
    EGLint attributes[] = {EGL_RED_SIZE,  8, EGL_GREEN_SIZE, 8,
			   EGL_BLUE_SIZE, 8, EGL_NONE};
    EGLConfig config;
    EGLint num_config;
    eglChooseConfig(_display, attributes, &config, 1, &num_config);

    // create context
    _context = eglCreateContext(_display, config, EGL_NO_CONTEXT, nullptr);

    // creaate egl sruface
    _egl_surface =
	eglCreateWindowSurface(_display, config, native_window, nullptr);

    eglMakeCurrent(_display, _egl_surface, _egl_surface, _context);
  }

  ~GlRenderer() {
    eglDestroySurface(_display, _egl_surface);
    eglDestroyContext(_display, _context);
    eglTerminate(_display);
  }

  auto clear() -> void {
    glClearColor(1.0f, 1.0f, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
  }

  // TODO: What Args?
  auto draw() {}

  auto swapBuffer() { eglSwapBuffers(_display, _egl_surface); }
};

} // namespace cx
