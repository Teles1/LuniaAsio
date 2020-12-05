#pragma once
#include "../Core/Core.h"
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
	#pragma region AllM Stupid Alive
	public:
		enum Marks { AliveReceived, AliveCleared };
		///	Alive structure alter Game Guard Object
		struct	Alive {
			Marks	Mark;

			///	backup alive data
			struct AliveData
			{
				uint32	index;
				uint32	value1;
				uint32	value2;
				uint32	value3;

				bool	operator==(AliveData& aliveData)
				{
					if (index == aliveData.index && value1 == aliveData.value1
						&& value2 == aliveData.value2 && value3 == aliveData.value3)
						return	true;

					return	false;
				}
			}answer, temp;
		}alive;
#pragma endregion
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
		~TcpConnection() {
			INFO_LOG("[] Disconnected", userID);
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

		void setMessageCallback(const MessageCallback& cb) { messageCallback_ = cb; }
		void setWriteCompleteCallback(const WriteCompleteCallback& cb) { writeCompleteCallback_ = cb; }
		void setDisconnectionCallback(const DisconnectionCallback& cb) { disconnectionCallback_ = cb; }

		void onMessage(const boost::system::error_code& error, size_t length)
		{
			//INFO_LOG("Packet[{0}][{1}][{2}]", length, EncryptKey?"true":"false", StringUtil::GetString(src, length, true));
			if (length == 0)
				return;
			if (!error)
			{
				auto t = std::thread([=]() {
					try {
						int total = 0;
						uint8* work = new uint8[Serializer::HeaderSize]{0};
						do {
							if (length - total >= Serializer::HeaderSize) {
								if (EncryptKey) {
									uint32 keyBackup = GetKey();
									//INFO_LOG("Work[{0}]", StringUtil::GetString(work, Serializer::HeaderSize));
									memcpy(work, src + total, Serializer::HeaderSize);
									//INFO_LOG("Work[{0}]", StringUtil::GetString(work, Serializer::HeaderSize));
									Translate(work, Serializer::HeaderSize); // translate the header only

									LengthType* work_length = reinterpret_cast<LengthType*>(work);
									LengthType pSize = *work_length;

									HashType* header_length = reinterpret_cast<HashType*>(work + 2);
									HashType header = *header_length;
									//printf("Psize[%.2X], Hash[%.2X]\n", pSize, header);
									if (header == Serializer::NetStreamHash) {
										//Translate only the bytes related to the packet
										if (pSize - Serializer::HeaderSize > length) {
											//Back up the packet for the next iteration. There is not enough bytes to parse this data, or, the client is trolling?
											WARN_LOG("Could not read entire packet. We should back up the packet for the next iteration WARN!!!!! pSize[{0}] > Length[{1}]", pSize - Serializer::HeaderSize, length);
										}
										else {
											//Packet can be deserialized safely.
											//Packet can be fully read because lenght > pSize
											Translate(&src[size_t(total) + size_t(Serializer::HeaderSize)], pSize - Serializer::HeaderSize); // Decryption done
											Parse(&src[size_t(total) + size_t(Serializer::HeaderSize)], pSize - Serializer::HeaderSize);
											total += pSize;
										}
										//length - total - HeaderSize > lenght  means that there is more data to be processed.
										// Eventually we gonna have to add a system to simple add data to a buffer or maybe a pointer to hold the data?
									}
									else {
										SetKey(keyBackup);
										total += 1;
									}
								}
								else { // If there is no cryptography we keep on adding 1 to the buffer until we find a NetStream
									LengthType* work_length = reinterpret_cast<LengthType*>(src + total);
									LengthType pSize = *work_length;

									HashType* header_length = reinterpret_cast<HashType*>(src + total + 2);
									HashType header = *header_length;
									//printf("NonCrypt Psize[%.2X], Hash[%.2X]\n", pSize, header);
									if (header == Serializer::NetStreamHash) {
										//printf("length[%d] total[%d] pSize[%d]\n", length, total, pSize);
										Parse(&src[size_t(total) + size_t(Serializer::HeaderSize)], pSize - Serializer::HeaderSize);
										total = +pSize;
									}
									else
										total += 1;
								}
								/*
								if (length - total < Serializer::HeaderSize) //Read Enough
									break;
								*/
							}
							else //Cant read header.
								break;
							/*
							else {
								WARN_LOG("Cannot read beyond stream");
								for (int i = 0; i < length; i++) {
									printf("%02x ", src[i]);
								}
								printf("\n");
								break;
							}*/
							if (total >= length)
								break;
						} while (true);
					}
					catch (...) {
						std::cout << "Exception trown\n"; 
					}

				}); 

				t.detach();
				//Gotta give the user's thread more read work to do.
				//doRead();
			}
			else if(error == boost::asio::error::eof)
			{
				disconnectionCallback_(this->userID);
				//std::cout << "User[" << this->getUserID() << "] disconnected." << std::endl;
			}
			else {
				ERROR_LOG(error.message());
			}
		}
		void Parse(uint8* buffer, size_t size) {
			messageCallback_(shared_from_this(), buffer, size);
			doRead();
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
		void UpdateAliveAuth(const Alive::AliveData& answer) {
			alive.Mark = AliveReceived;
		}
#pragma region Crypt
		void SetKey(uint32 key) {
			decryptor.SetKey(key);
			EncryptKey = true;
		}
		uint32 GetKey() {
			return decryptor.GetKey();
		}
		void Translate(void* source, uint32 bytes) {
			decryptor.Translate(source, bytes);
		}
#pragma endregion
	private:
		void doRead() 
		{
			socket_.async_read_some(boost::asio::buffer(src, bytes_read), std::bind(&TcpConnection::onMessage, this, std::placeholders::_1, std::placeholders::_2));
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
		uint8 src[MAX_BUFFER_SIZE];
		size_t bytes_read;
		MessageCallback messageCallback_;
		WriteCompleteCallback writeCompleteCallback_;
		DisconnectionCallback disconnectionCallback_;

		#pragma endregion
	};

}

class SerialPort
{
public:
	explicit SerialPort(const std::string& portName) :
		m_startTime(std::chrono::system_clock::now()),
		m_readBuf(new char[bufSize]),
		m_ios(),
		m_ser(m_ios)
	{
		m_ser.open(portName);
		m_ser.set_option(boost::asio::serial_port_base::baud_rate(115200));

		auto readHandler = [&](const boost::system::error_code& ec, std::size_t bytesRead)->void
		{
			// Need to pass lambda as an input argument rather than capturing because we're using auto storage class
			// so use trick mentioned here: http://pedromelendez.com/blog/2015/07/16/recursive-lambdas-in-c14/
			// and here: https://stackoverflow.com/a/40873505
			auto readHandlerImpl = [&](const boost::system::error_code& ec, std::size_t bytesRead, auto& lambda)->void
			{
				if (!ec)
				{
					const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - m_startTime);

					std::cout << elapsed.count() << "ms: " << std::string(m_readBuf.get(), m_readBuf.get() + bytesRead) << std::endl;

					// Simulate some kind of intensive processing before re-registering our read handler
					std::this_thread::sleep_for(std::chrono::seconds(5));

					//m_ser.async_read_some(boost::asio::buffer(m_readBuf.get(), bufSize), lambda);
					m_ser.async_read_some(boost::asio::buffer(m_readBuf.get(), bufSize), std::bind(lambda, std::placeholders::_1, std::placeholders::_2, lambda));
				}
			};

			readHandlerImpl(ec, bytesRead, readHandlerImpl);
		};

		m_ser.async_read_some(boost::asio::buffer(m_readBuf.get(), bufSize), readHandler);

		m_asioThread = std::make_unique<std::thread>([this]()
			{
				this->m_ios.run();
			});
	}

	~SerialPort()
	{
		m_ser.cancel();
		m_asioThread->join();
	}

private:
	const std::chrono::system_clock::time_point m_startTime;
	static const std::size_t bufSize = 512u;
	std::unique_ptr<char[]> m_readBuf;
	boost::asio::io_service m_ios;
	boost::asio::serial_port m_ser;
	std::unique_ptr<std::thread> m_asioThread;
};
