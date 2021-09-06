

#pragma once

#include <thread>
#include <vector>
#include <ubm.h>
#include <ubm_common.h>

namespace ubm {

struct Device : public raii<ubm_device *> {};
struct Source : public raii<ubm_source *> {};

auto destroy(ubm_device *dev) -> void { ubm_device_destroy(dev); }

auto create(int appid, int port, int domain, int type) -> Device {
  auto dev = ubm_device_create(appid, port, domain, type);
  return Device(dev, [](auto d) { destroy(d); });
}

} // namespace ubm

namespace cx {

struct Producer {};

struct Consumer {
public:
  Consumer(ubm::Device device) : _cancelationToken(false) {}
  ~Consumer() {
    _cancelationToken = true;
    _consumer.join();
  }

  auto func() {
    while (!_cancelationToken) {
      struct ubm_source_bo *bo = NULL;
      ret = ubm_source_acquire_buffer(source, &bo, 3000);

      struct ubm_buffer_info buffer_info;
      ret = ubm_source_get_buffer_info(prod.usource, bo, &buffer_info);

      // TODO: validate buffer info
      struct ubm_dmabuf_info dmabuf_info;
      ret = ubm_export_dmabuf_fd(&buffer_info, &dmabuf_info);

      prod.buffer_q.push(buffer);
    }
  }

private:
  bool _cancelationToken;
  std::thread _consumer;
}

class UbmFrontend {

  auto addProducer(int appid, int port) -> void {
    auto ubmDevice = ubm_device_create(appid, port, UBM_LOCAL_DOMAIN, UBM_CONS);
    auto ubmSource = ubm_source_create(ubmDevice);
    if (!ubmSource) {
      ubm_device_destroy(ubmDevice);
      throw std::runtime_error(fmt::format(
	  "Failed to created ubm_source. appid={}, port={}", appid, port));
    }

    struct ubm_buffer_info info;
    auto ret = ubm_source_get_buffer_info(ubmSource, NULL, &info);
    if (ret != UBM_ERROR_NONE) {
      throw std::runtime_error(fmt::format(
	  "Failed to get ubm_buffer_info. appid={}, port={}", appid, port));
    }

    ubm_source_acquire_buffer(prod.usource, &bo, 3000);
  }
};
} // namespace cx

