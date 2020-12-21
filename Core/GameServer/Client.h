#pragma once

#include "NetworkIO/ClientNetworkIO.h"

struct Client : public ClientNetworkIO //, TODO? std::enable_shared_from_this<Client>
{
	Client(asio::ip::tcp::socket&& socket);

	~Client() { };

public:
	fwEvent<> OnDropped;

private:

	uint32_t id;
};