
#include "backend.hpp"
#include <GLES3/gl3.h>
#include <GLES2/gl2ext.h>

namespace lunar {
namespace backend {

Backend::Backend(EGLenum platform, void *native_eglDisplay,
		 void *native_window) {

  _eglDisplay = egl::getPlatformDisplay(platform, native_eglDisplay, nullptr);
  if (_eglDisplay == EGL_NO_DISPLAY) {
    throw std::runtime_error("Failed to get platform display.");
  }

  if (!eglInitialize(_eglDisplay, nullptr, nullptr))
    throw std::runtime_error("Failed to initailize EGL");

  // create an OpenGL context
  if (!eglBindAPI(EGL_OPENGL_ES_API))
    throw std::runtime_error("Failed to Bind API");

  // config
  EGLint config_attribs[] = {EGL_SURFACE_TYPE,
			     EGL_WINDOW_BIT,
			     EGL_RED_SIZE,
			     1,
			     EGL_GREEN_SIZE,
			     1,
			     EGL_BLUE_SIZE,
			     1,
			     EGL_ALPHA_SIZE,
			     1,
			     EGL_RENDERABLE_TYPE,
			     EGL_OPENGL_ES2_BIT,
			     EGL_NONE};

  EGLConfig config;
  EGLint num_config;
  eglChooseConfig(_eglDisplay, config_attribs, &config, 1, &num_config);

  // create context
  static const EGLint context_attribs[] = {EGL_CONTEXT_CLIENT_VERSION, 2,
					   EGL_NONE};
  _eglContext =
      egl::createContext(_eglDisplay, config, EGL_NO_CONTEXT, context_attribs);
  if (_eglContext == EGL_NO_CONTEXT) {
    throw std::runtime_error(fmt::format(
	"ERROR: Failed to create a EGL context. {:#x}", eglGetError()));
  }

  if (native_window) {
    // create egl sruface
    _eglSurface = egl::createPlatformWindowSurfaceExt_(_eglDisplay, config,
						       native_window, nullptr);
    if (_eglSurface == EGL_NO_SURFACE) {
      throw std::runtime_error(fmt::format(
	  "ERROR: Failed to create a EGL surface. {:#x}", eglGetError()));
    }
  }

  eglMakeCurrent(_eglDisplay, _eglSurface, _eglSurface, _eglContext);
}

Backend::~Backend() {}

auto Backend::bind() -> void {
  eglMakeCurrent(_eglDisplay, _eglSurface, _eglSurface, _eglContext);
}

auto Backend::swap() -> void {
  if (_eglSurface == EGL_NO_SURFACE) {
    glFinish();
  } else {
    eglSwapBuffers(_eglDisplay, _eglSurface);
  }
}

auto Backend::createFramebuffer(Buffer const &b) -> gl::Framebuffer {
  std::cout << fmt::format("Create buffer. width={} ,height={}", b.width,
			   b.height)
	    << std::endl;
  GLint attribs[] = {EGL_WIDTH,
		     b.width,
		     EGL_HEIGHT,
		     b.height,
		     EGL_LINUX_DRM_FOURCC_EXT,
		     b.format,
		     EGL_DMA_BUF_PLANE0_FD_EXT,
		     b.planes[0].fd,
		     EGL_DMA_BUF_PLANE0_OFFSET_EXT,
		     b.planes[0].offset,
		     EGL_DMA_BUF_PLANE0_PITCH_EXT,
		     b.planes[0].stride,
		     EGL_NONE};

  auto eglImage = egl::createImageKHR(_eglDisplay, EGL_NO_CONTEXT,
				      EGL_LINUX_DMA_BUF_EXT, NULL, attribs);

  auto texture = gl::genTexture();
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  static auto glEGLImageTargetTexture2DOES =
      (PFNGLEGLIMAGETARGETTEXTURE2DOESPROC)eglGetProcAddress(
	  "glEGLImageTargetTexture2DOES");
  if (!glEGLImageTargetTexture2DOES) {
    throw std::runtime_error(
	"EGL doesn't support glEGLImageTargetTexture2DOES");
  }

  glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, eglImage);

  auto framebuffer = gl::genFramebuffer();
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
			 texture, 0);

  auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (status != GL_FRAMEBUFFER_COMPLETE)
    throw std::runtime_error(fmt::format(
	"Failed to create a framebuffer. status={:#x}, glError={:#x}", status,
	glGetError()));

  // XXX: depends on a texture
  framebuffer.dependOn(texture);
  framebuffer.dependOn(eglImage);

  return framebuffer;
}

auto Backend::createTexture(Buffer const &b) -> gl::Texture {
  GLint attribs[] = {EGL_WIDTH,
		     b.width,
		     EGL_HEIGHT,
		     b.height,
		     EGL_LINUX_DRM_FOURCC_EXT,
		     b.format,
		     EGL_DMA_BUF_PLANE0_FD_EXT,
		     b.planes[0].fd,
		     EGL_DMA_BUF_PLANE0_OFFSET_EXT,
		     b.planes[0].offset,
		     EGL_DMA_BUF_PLANE0_PITCH_EXT,
		     b.planes[0].stride,
		     EGL_NONE};

  auto eglImage = egl::createImageKHR(_eglDisplay, EGL_NO_CONTEXT,
				      EGL_LINUX_DMA_BUF_EXT, NULL, attribs);

  auto texture = gl::genTexture();
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  static auto glEGLImageTargetTexture2DOES =
      (PFNGLEGLIMAGETARGETTEXTURE2DOESPROC)eglGetProcAddress(
	  "glEGLImageTargetTexture2DOES");
  if (!glEGLImageTargetTexture2DOES) {
    throw std::runtime_error(
	"EGL doesn't support glEGLImageTargetTexture2DOES");
  }

  glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, eglImage);

  texture.dependOn(eglImage);
  return texture;
}

} // namespace backend

} // namespace lunar
