#pragma once
#include "fwEvent.h"
#include <iostream>
#include "Utils/InitFunction.h"

#include <LobbyServer/fwPacketListener.h>

#include <asio.hpp>
#include <asio/basic_stream_socket.hpp>

struct GameServerIOService
{
	GameServerIOService(const char* addr, unsigned port_num) :
		m_acceptor(m_ioService, asio::ip::tcp::endpoint(asio::ip::address_v4::any(), port_num)) // this ip is what asio will be listening for :')
		, m_socket(m_ioService)
	{ 
		m_acceptor.async_accept(m_socket, std::bind(&GameServerIOService::AcceptorAsyncAcceptHandler, this, std::placeholders::_1));
	
		// m_ioService.run();

		std::cout << "GameServerIOService is not running" << std::endl;
	}

	~GameServerIOService() { };

private:
	void AcceptorAsyncAcceptHandler(const asio::error_code& err_code)
	{
		std::cout << "OnSocketConnectionCreated was triggered err_code " << err_code << std::endl;

		// OnSocketConnectionCreated(err_code, m_socket);

		m_acceptor.async_accept(m_socket, std::bind(&GameServerIOService::AcceptorAsyncAcceptHandler, this, std::placeholders::_1));
	}

public:
	// fwEvent<> OnAcceptorCreated;

	fwEvent<const asio::error_code&, asio::ip::tcp::socket&> OnSocketConnectionCreated;

	fwEvent<const asio::error_code&, asio::ip::tcp::socket&> OnSocketConnectionDropped;

private:
	asio::io_service m_ioService;

	asio::ip::tcp::acceptor m_acceptor;

	asio::ip::tcp::socket m_socket;
};

/////////////////////////////////
////////// GAMESERVER ///////////
/////////////////////////////////

template<typename TServerProxy>
class GameServer : GameServerIOService
{
public:
	GameServer(const char* addr, unsigned port_num) : GameServerIOService(addr, port_num)
	{
		Proxy = new TServerProxy();

		OnSocketConnectionCreated.Connect([&/*TODO Pass Proxy ref*/](const asio::error_code& err_code, asio::ip::tcp::socket& socket)
		{
			// Proxy->

			std::cout << "Socket Connection Created" << std::endl;

		});
	};

	~GameServer() { };

public:
	TServerProxy* Proxy;

private:
	/*
		make fwPacketListener use the () operator instead of ::Invoke
	*/
};

/////////////////////////////////
////////// CLIENT ///////////////
/////////////////////////////////

struct ClientIOSocket
{

public:
	fwEvent<> OnSocketMessage;
};

struct Client : ClientIOSocket //, enable_shared_from_this
{
	Client() { };

	~Client() { };

public:
	fwEvent<> OnDropped;

private:

	uint32_t id;
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

/////////////////////////////////
/////// CLIENT REGISTRY /////////
/////////////////////////////////

template<typename TClientScope>
struct ClientRegistry
{
private:
	typedef std::shared_ptr<TClientScope> ClientSharedPtr;
	typedef std::weak_ptr  <TClientScope> ClientWeakPtr;

public:
	ClientRegistry() { };

	~ClientRegistry() { };

	void MakeClient()
	{
		ClientSharedPtr client = std::make_shared<TClientScope>();

		OnClientCreated(client);
	}

	void DropClient(ClientSharedPtr& client)
	{
		client->OnDropped();

		OnClientDropped(client);
	}

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

/////////////////////////////////
//////// SERVERPROXY ////////////
/////////////////////////////////

template<typename TClientScope>
struct ServerProxy // : std::enable_shared_from_this
{
public:
	ServerProxy() 
	{
		m_clientRegistry.OnClientCreated.Connect([&/*TODO Pass PacketListener ref*/](std::shared_ptr<TClientScope>& client)
		{
			client->OnSocketMessage.Connect([/*TODO Pass PacketListener ref*/]()
			{
				// PacketListener.Invoke();
			});
		});
	};

	~ServerProxy() { };

public:
	ClientRegistry<TClientScope>& GetClientRegistry()
	{
		// lock
		return m_clientRegistry;
		// unlock
	}

public:
	Lunia::fwPacketListener PacketListener;

private:
	ClientRegistry<TClientScope> m_clientRegistry;

};

/////////////////////////////////
///// SERVERPROXY PROXIES ///////
/////////////////////////////////

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