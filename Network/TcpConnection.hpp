#pragma once
#include <memory>
#include "./Network.h"
#include "../Core/Common.h"
#include "Callbacks.hpp"
namespace net
{
	class TcpConnection : public std::enable_shared_from_this<TcpConnection>
	{
	public:
		using Tcp = boost::asio::ip::tcp;
	public:
		TcpConnection(Tcp::socket&& socket) :
			socket_(std::move(socket)),
			messageCallback_(defaultMessageCallback),
			writeCompleteCallback_(defaultWriteCompleteCallback),
			cryptoKey(0)
		{
		}
		/*
			User's first connection gets started by this function.
		*/
		void start(uint32 userID)
		{
			this->userID = userID;
			doRead();
		}

		void asyncSend(const void* data, size_t len)
		{
			boost::asio::async_write(socket_, boost::asio::buffer(data, len),
			std::bind(&TcpConnection::onWriteComplete, this, std::placeholders::_1, std::placeholders::_2));
		}
		void asyncSend(const std::string& content)
		{
			boost::asio::async_write(socket_, boost::asio::buffer(content.data(), content.size()),
				std::bind(&TcpConnection::onWriteComplete, this, std::placeholders::_1, std::placeholders::_2));
		}
		bool send(const void* data, size_t len)
		{
			try
			{
				boost::asio::write(socket_, boost::asio::buffer(data, len));
			}
			catch (const std::exception& e)
			{
				std::cerr << e.what() << std::endl;
				return false;
			}
			return true;
		}
		bool send(const std::string& content)
		{
			try
			{
				boost::asio::write(socket_, boost::asio::buffer(content.data(), content.size()));
			}
			catch (const std::exception& e)
			{
				std::cerr << e.what() << std::endl;
				return false;
			}
			return true;
		}

		void setMessageCallback(const MessageCallback& cb) { messageCallback_ = cb; }
		void setWriteCompleteCallback(const WriteCompleteCallback& cb) { writeCompleteCallback_ = cb; }
		void setDisconnectionCallback(const DisconnectionCallback& cb) { disconnectionCallback_ = cb; }

		void onMessage(const boost::system::error_code& error, size_t len)
		{
			if (!error)
			{
				auto t = std::thread([=]() { messageCallback_(shared_from_this(), buf, len); }); 

				t.detach();
				//Gotta give the user's "thread" some work
				doRead();
			}
			else if(error == boost::asio::error::eof)
			{
				disconnectionCallback_(this->userID);
				//std::cout << "User[" << this->getUserID() << "] disconnected." << std::endl;
			}
		}
		void onWriteComplete(const boost::system::error_code& error, size_t len)
		{
			if (!error)
			{
				writeCompleteCallback_(shared_from_this(), len);
			}
			else if(error == boost::asio::error::eof)
			{
				std::cout << "[" << this->getUserID() <<"]Error Writing data => " << std::endl;
			}
		}

		std::string peerAddress() const 
		{ 
			return socket_.remote_endpoint().address().to_string(); 
		}
		unsigned short peerPort() const 
		{ 
			return socket_.remote_endpoint().port(); 
		}
		std::string peerIpPort() const  
		{
			return std::string(peerAddress() + ":" + std::to_string(peerPort()));
		};
		uint32 getUserID() const {
			return this->userID;
		}
	private:
		void doRead() 
		{
			socket_.async_read_some(boost::asio::buffer(buf),
			std::bind(&TcpConnection::onMessage, this, std::placeholders::_1, std::placeholders::_2));
		}
#pragma region Game related
	private:
		uint32 cryptoKey;
#pragma endregion
		#pragma region Network Related
	private:
		boost::asio::ip::tcp::socket socket_;
		static const int kBufferSize = MAX_BUFFER_SIZE;
		uint8 data_[kBufferSize];
		uint32 userID = 0;
		std::array<uint8, MAX_BUFFER_SIZE> buf;

		MessageCallback messageCallback_;
		WriteCompleteCallback writeCompleteCallback_;
		DisconnectionCallback disconnectionCallback_;

		#pragma endregion
	};
}