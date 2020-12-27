#pragma once

#include "../NetworkIO.h"

struct GameServerNetworkIO
{
	GameServerNetworkIO(const char* addr, uint16_t port_num);

	~GameServerNetworkIO() { };

	void RunNetworkIOService();

private:

	void MakeAcceptorAsyncAcceptHandler();

	void AcceptorAsyncAcceptHandler(const asio::error_code& err_code);

public:
	// fwEvent<> OnAcceptorCreated;

	fwEvent<const asio::error_code&, asio::ip::tcp::socket&> OnSocketConnectionCreated;

	// fwEvent<const asio::error_code&, asio::ip::tcp::socket&&> OnSocketConnectionDropped;

private:
	asio::io_service m_ioService;

	asio::ip::tcp::acceptor m_acceptor;

	asio::ip::tcp::socket m_socket;
};

