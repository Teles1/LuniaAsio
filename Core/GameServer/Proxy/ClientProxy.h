#pragma once

#include "../NetworkIO/ClientNetworkIO.h"

struct ClientProxy : public ClientNetworkIO //, TODO? std::enable_shared_from_this<Client>
{
	ClientProxy(asio::ip::tcp::socket&& socket);

	~ClientProxy() { };

public:
	fwEvent<> OnDropped;

private:

	uint32_t id;
};