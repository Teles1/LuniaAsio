#ifndef Shared_GUARD
#define Shared_GUARD
#include <boost/asio.hpp>
#include <boost/asio/basic_stream_socket.hpp>
#include <iostream>

typedef unsigned char uint8;

namespace Shared {
    using ReceivedCallback = std::function<void(std::vector<uint8>& buffer)>;
}
namespace net {
    namespace ba = boost::asio;
    using ba::ip::tcp;
    using error_code = boost::system::error_code;
}
#endif // !Shared_GUARD