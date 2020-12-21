#pragma once

#include "../ServerProxy.h"

#include "../ClientProxy/ClientProxyLobby.h"

struct ServerProxyLobby : public ServerProxy<ClientProxyLobby>
{
public:
	void Say()
	{
		std::cout << "ServerProxyLobby :: Say" << std::endl;
	};
};

/*
struct ServerProxyStage : public ServerProxy<ClientProxyStage>
{
public:
	void Say()
	{
		std::cout << "ServerProxyStage :: Say" << std::endl;
	};
};

struct ServerProxyPvp : public ServerProxy<ClientProxyPvp>
{
public:
	void Say()
	{
		std::cout << "ServerProxyPvp :: Say" << std::endl;
	};
};
*/