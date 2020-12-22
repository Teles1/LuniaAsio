#pragma once

#include <Core/GameServer/Proxy/ClientProxy.h>

struct ClientProxyLobby : public ClientProxy
{
	ClientProxyLobby(asio::ip::tcp::socket&& socket); /* WARN socket is moved a shit ton of times here */
};