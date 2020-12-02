#pragma once
#include "../Core/Common.h"
#include "./Network.h"
#include "Crypt/Crypt.h"
#include "NetStream/NetStream.h"
#include "./CallBacks.hpp"
namespace Network
{
	/*
		Please, DO NOT define bytes_read and buffer. It sets it to default value everytime.
	*/
	class TcpConnection : public std::enable_shared_from_this<TcpConnection>
	{
	public:
		using Tcp = boost::asio::ip::tcp;
		TcpConnection(Tcp::socket&& socket) :
			socket_(std::move(socket)),
			messageCallback_(defaultMessageCallback),
			writeCompleteCallback_(defaultWriteCompleteCallback),
			decryptor(false),
			EncryptKey(false)
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

		void onMessage(const boost::system::error_code& error, size_t length)
		{
			INFO_LOG("Message received[{0}]", length);
			if (!error)
			{
				auto t = std::thread([=]() {
					try {
						int total = 0;
						int result = 0;
						do {
							//check if still connected.
							if (length - total > sizeof(LengthType)) {
								if (EncryptKey) {
									unsigned char* work = &buffer[total]; // starting from where we left after running the loop n times
									this->Translate(work, sizeof(LengthType)); // translate the header only
									LengthType* work_length = reinterpret_cast<LengthType*>(work);

									//backup to get roll back
									LengthType backup = *work_length;
									uint32 key = this->GetEncryptKey(); // key gonna be changed right after Translate

									// partial decryption

									LengthType size = *work_length;
									if (size < sizeof(LengthType)) throw;
									if (size > length - total) { // not big enought to be parsed
										this->SetEncryptKey(key);
										*work_length = backup;
										break;
									}

									this->Translate(work + sizeof(LengthType), size - sizeof(LengthType));
								}
								//In here the message should be ready to be readed.
								//Might get stuck in here if message was broken in half. lol
								result = this->Parse(&buffer[total] , length - total);
							}
							total += result;
							std::cout << "Reading packet\n";
						} while (result);
					}
					catch (...) { std::cout << "Exception trown\n"; }


				}); 

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
		int Parse(uint8* buffer, size_t size) {
			INFO_LOG("Trying to Parse Message[{0}]", size);
			if (size < sizeof(HashType) + sizeof(LengthType))
				return 0;
			NetStream reader(buffer, size);
			int read = reader.GetNetStreamSize();
			if (read == 0)
				return 0;
			if (read < 0) {
				ERROR_LOG("[{0}]Invalid packet", userID);
				return size;
			}
			if (reader.IsNetStream() && read <= size) {
				messageCallback_(shared_from_this(), &buffer[GetNetStreamHeaderSize()], size - GetNetStreamHeaderSize());
				return read;
			}
			else
				return 0;
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
#pragma region Crypt
		void SetEncryptKey(uint32 key) {
			decryptor.SetKey(key);
			EncryptKey = true;
		}
		uint32 GetEncryptKey() {
			return decryptor.GetKey();
		}
		void Translate(void* source, uint32 bytes) {
			decryptor.Translate(source, bytes);
		}
#pragma endregion
	private:
		void doRead() 
		{
			socket_.async_receive(boost::asio::buffer(buffer, bytes_read), std::bind(&TcpConnection::onMessage, this, std::placeholders::_1, std::placeholders::_2));
		}
#pragma region Game related
	private:
		Network::Crypt::Box decryptor;
		bool EncryptKey;
#pragma endregion
		#pragma region Network Related
	private:
		boost::asio::ip::tcp::socket socket_;
		uint32 userID = 0;
		std::array<uint8, MAX_BUFFER_SIZE> buffer;
		size_t bytes_read;
		MessageCallback messageCallback_;
		WriteCompleteCallback writeCompleteCallback_;
		DisconnectionCallback disconnectionCallback_;

		#pragma endregion
	};

}