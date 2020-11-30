#include <iostream>
#include <string>
#include <thread>
#include "StdAfx.h"
#include "TcpConnection.h"

using namespace std;
using namespace net;
using namespace std::placeholders;

TcpConnection::TcpConnection(Tcp::socket&& socket):
	socket_(std::move(socket)),
	messageCallback_(defaultMessageCallback),
	writeCompleteCallback_(defaultWriteCompleteCallback)
{
}

void TcpConnection::start()
{
	/*try {
		for (;;)
		{
			asio::error_code error;

			//size_t len = socket_.read_some(asio::buffer(buf), error);

			/*if (error == asio::error::eof)
				break; // Connection closed cleanly by peer.
			else if (error)
				throw asio::system_error(error); // Some other error.

			std::cout.write(buf.data(), len);
			std::cout << "\n" << std::endl;
		}
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}*/
	doRead();
}

void TcpConnection::doRead()
{
	socket_.async_read_some(asio::buffer(buf),
							std::bind(&TcpConnection::onMessage, this, _1, _2));
}


void TcpConnection::asyncSend(const void* data, size_t len)
{
	asio::async_write(socket_, asio::buffer(data, len),
		std::bind(&TcpConnection::onWriteComplete, this, _1, _2));
}

void TcpConnection::asyncSend(const std::string& content)
{
	asio::async_write(socket_, asio::buffer(content.data(), content.size()),
							 std::bind(&TcpConnection::onWriteComplete, this, _1, _2));
}

bool TcpConnection::send(const void* data, size_t len)
{
	try
	{
		asio::write(socket_, asio::buffer(data, len));
	}
	catch (const std::exception& e)
	{
		cerr << e.what() << endl;
		return false;
	}
	return true;
}

bool TcpConnection::send(const std::string& content)
{
	try
	{
		asio::write(socket_, asio::buffer(content.data(), content.size()));
	}
	catch (const std::exception& e)
	{
		cerr << e.what() << endl;
		return false;
	}
	return true;
}

void TcpConnection::onMessage(const asio::error_code& error, size_t len)
{
	if (!error)
	{
		auto t = std::thread([=](){
			messageCallback_(shared_from_this(), buf, len);
		});

		t.detach();

		doRead();
	}
}

void TcpConnection::onWriteComplete(const asio::error_code& error, size_t len)
{
	if (!error)
	{
		writeCompleteCallback_(shared_from_this(), len);
	}
}