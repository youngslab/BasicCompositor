
#pragma once

#include "acrab_message.hpp"
#include <gnf/client.hpp>

namespace acrab {

using SocketType = boost::asio::local::stream_protocol;
class client : gnf::GenericClient<SocketType, Message> {

  void onMessageSent(const gnf::Message<Message> &message) override {
    switch (message.header.type) {
    case Message::Buffer:
      break;
    default:
      break;
    }
  }

  auto send(Buffer
};

} // namespace acrab
