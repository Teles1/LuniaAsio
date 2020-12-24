#pragma once

#include "../NetworkIO.h"

#include <Network/Crypt/Crypt.h>

struct ClientNetworkIO : std::enable_shared_from_this<ClientNetworkIO> /* TODO Is it necessary? */
{
	static const uint16_t READ_BUFFER_LENGTH = 2 << 12;
	static const uint16_t PACKET_HEADER_SIZE = sizeof(uint16_t) + sizeof(unsigned short);
	static const uint16_t NETSTREAM_HASHED   = (uint16_t) 0x55E0;

	ClientNetworkIO(asio::ip::tcp::socket&& socket);

	~ClientNetworkIO() { std::cout << "~ClientNetworkIO" << std::endl; };

private:
	void MakeSocketAsyncReadSome();

	void SocketAsyncReadSome(const asio::error_code& ec, size_t size);

	void SocketAsyncWriteSome(const asio::error_code& ec, size_t size);

public:
	void MakeSocketAsyncWriteSome(uint8_t * buffer, size_t size);

	void Drop();

	void SetEncryptionKey(uint32_t& key) { };

	bool HasEncryptionKey() const
	{
		return m_hasEncryptionKey;
	};

	std::string GetPeerAddress() const
	{
		return "127.0.0.1"; // m_socket.remote_endpoint().address().to_string();
	};

public:
	fwEvent<char*, unsigned short&> OnSocketReadPacket;

private:

	asio::ip::tcp::socket m_socket;

	Lunia::Net::Crypt::Box m_decryptor; /* TODO Should probably be a singleton */

	char m_buffer[READ_BUFFER_LENGTH];

	bool m_hasEncryptionKey = false;
};
