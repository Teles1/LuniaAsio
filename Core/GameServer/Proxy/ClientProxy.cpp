#include "ClientProxy.h"

ClientProxy::ClientProxy(asio::ip::tcp::socket&& socket) : ClientNetworkIO(std::move(socket)) /* WARN socket is moved a shit ton of times here */
{
	std::cout << "ClientProxy : constructor called" << std::endl;
};