#pragma once
#include "fwEvent.h"
#include <iostream>
#include "Utils/InitFunction.h"

#include <LobbyServer/fwPacketListener.h>

template<typename TServerProxy>
class GameServer
{
public:
	GameServer() 
	{
		OnAcceptorCreated();
	};

	~GameServer() { };

public:

	TServerProxy* Proxy;

	fwEvent<> OnAcceptorCreated;

	fwEvent<> OnSocketConnectionCreated;

	fwEvent<> OnSocketConnectionDropped;

	Lunia::fwPacketListener PacketListener;

public:


private:

	/*
		make fwPacketListener use the () operator instead of ::Invoke
	*/
};

struct Client // : std::enable_shared_from_this
{
	Client() { };

	~Client() { };
};

struct ClientProxyLobby : Client
{

};

struct ClientProxyStage : Client
{

};

struct ClientProxyPvp : Client
{

};

template<typename TClientScope>
struct ClientRegistry
{
private:
	typedef std::shared_ptr<TClientScope> ClientSharedPtr;
	typedef std::weak_ptr  <TClientScope> ClientWeakPtr;

public:
	ClientRegistry() { };

	~ClientRegistry() { };

public:

	fwEvent<ClientSharedPtr&> OnClientCreated;

	fwEvent<ClientSharedPtr&> OnClientDropped;

public:

	template<typename F>
	void ForAllClients(F f)
	{

	}

private:

	std::vector<ClientSharedPtr> m_clients;
};

template<typename TClientScope>
struct ServerProxy // : std::enable_shared_from_this
{
public:
	ServerProxy() { };

	~ServerProxy() { };

public:

	ClientRegistry<TClientScope>& GetClientRegistry()
	{
		// lock
		return m_clientRegistry;
		// unlock
	}

private:
	ClientRegistry<TClientScope> m_clientRegistry;
};

struct ServerProxyLobby : ServerProxy<ClientProxyLobby>
{
public:
	void Say()
	{
		std::cout << "ServerProxyLobby :: Say" << std::endl;
	};
};

struct ServerProxyStage : ServerProxy<ClientProxyStage>
{
public:
	void Say()
	{
		std::cout << "ServerProxyStage :: Say" << std::endl;
	};
};

struct ServerProxyPvp : ServerProxy<ClientProxyPvp>
{
public:
	void Say()
	{
		std::cout << "ServerProxyPvp :: Say" << std::endl;
	};
};