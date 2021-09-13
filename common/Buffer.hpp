
#pragma once

#include <GLES2/gl2.h>
#include <vector>

namespace lunar {

struct Plane {
  int fd;
  GLint stride;
  GLint offset;
};

struct Buffer {
  int format;
  GLint width;
  GLint height;
  std::vector<Plane> planes;
};

} // namespace lunar
