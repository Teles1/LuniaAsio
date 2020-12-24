#pragma once

#include "NetworkIO/GameServerNetworkIO.h"

template<typename TServerProxy>
class GameServer : public GameServerNetworkIO
{
public:
	GameServer(const char* addr, uint16_t port_num) : GameServerNetworkIO(addr, port_num)
	{
		Proxy = new TServerProxy();

		OnSocketConnectionCreated.Connect([&/*TODO Pass Proxy ref*/](const asio::error_code& err_code, asio::ip::tcp::socket& socket)
		{
			// Proxy->

			std::cout << "Socket Connection Created " << std::endl;

			Proxy->ClientRegistry.MakeClient(std::move(socket)); /* WARN socket is moved a shit ton of times here */
		});
	};

	~GameServer() { };

public:
	TServerProxy* Proxy;

private:
	/*
		TODO make fwPacketListener use the () operator instead of ::Invoke
	*/
};