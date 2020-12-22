#include "ClientProxyLobby.h"

ClientProxyLobby::ClientProxyLobby(asio::ip::tcp::socket&& socket) : ClientProxy(std::move(socket))
{

}; /* WARN socket is moved a shit ton of times here */