#ifndef Shared_GUARD
#define Shared_GUARD

#define ASIO_STANDALONE
#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif

#include <boost/asio.hpp>
#include <boost/asio/basic_stream_socket.hpp>
#include <iostream>
#include "../Core/Core.h"

namespace Shared {
    using ReceivedCallback = std::function<void(std::vector<uint8>& buffer)>;
}
namespace net {
    namespace ba = boost::asio;
    using ba::ip::tcp;
    using error_code = boost::system::error_code;
    const uint16 READ_DATA_BUFFER_LENGTH = 2 << 12;
}
#include "../Core/fwEvent.h"
#endif // !Shared_GUARD