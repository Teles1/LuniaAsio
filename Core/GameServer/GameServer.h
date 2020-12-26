#pragma once

#include "NetworkIO/GameServerNetworkIO.h"

template<typename TServerProxy>
class GameServer : public GameServerNetworkIO
{
public:
	GameServer(const char* addr, uint16_t port_num) : GameServerNetworkIO(addr, port_num)
	{
		// Proxy = new TServerProxy();

		OnSocketConnectionCreated.Connect([Proxy = &Proxy](const asio::error_code& err_code, asio::ip::tcp::socket& socket)
		{
			Proxy->ClientRegistry.MakeClient(std::move(socket)); /* WARN socket is moved a shit ton of times here */
		});
	};

	~GameServer() { };

public:
	TServerProxy Proxy;

private:
	/*
		TODO make fwPacketListener use the () operator instead of ::Invoke
	*/
};