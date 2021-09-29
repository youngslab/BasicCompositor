

#pragma once

#include <bits/stdint-intn.h>
#include <vector>

namespace acrab {

// body 8 bytes + contorl 4 byte
struct Plane {
  int32_t fd;
  int32_t stride;
  int32_t offset;
};

struct Buffer {
  int32_t format;
  int32_t width;
  int32_t height;
  std::vector<Plane> planes;
};
} // namespace acrab
