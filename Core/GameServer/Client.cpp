#include "Client.h"

Client::Client(asio::ip::tcp::socket&& socket) : ClientNetworkIO(std::move(socket)) /* WARN socket is moved a shit ton of times here */
{
	std::cout << "Client : constructor called" << std::endl;
};