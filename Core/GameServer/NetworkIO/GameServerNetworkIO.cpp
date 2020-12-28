#include "GameServerNetworkIO.h"

GameServerNetworkIO::GameServerNetworkIO(const char* addr, uint16_t port_num) :
	m_acceptor(m_ioService, asio::ip::tcp::endpoint(asio::ip::address_v4::any(), port_num)) // this ip is what asio will be listening for :')
	, m_socket(m_ioService)
{
	MakeAcceptorAsyncAcceptHandler();
}

void GameServerNetworkIO::RunNetworkIOService()
{
	m_ioService.run();

	std::cout << "GameServerNetworkIOService has stopped" << std::endl;
}

void GameServerNetworkIO::MakeAcceptorAsyncAcceptHandler()
{
	m_acceptor.async_accept(m_socket, std::bind(&GameServerNetworkIO::AcceptorAsyncAcceptHandler, this, std::placeholders::_1));
}

void GameServerNetworkIO::AcceptorAsyncAcceptHandler(const asio::error_code& err_code)
{
	if (!err_code)
	{
		OnSocketConnectionCreated(err_code, m_socket); /* WARN socket is moved a shit ton of times here */
	}
	else
	{

	}

	MakeAcceptorAsyncAcceptHandler();
}