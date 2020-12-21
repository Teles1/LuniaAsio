#pragma once

#include "../NetworkIO.h"

#include <Network/Crypt/Crypt.h>

struct ClientNetworkIO : std::enable_shared_from_this<ClientNetworkIO> /* TODO Is it necessary? */
{
	static const uint16_t READ_BUFFER_LENGTH = 2 << 12;
	static const uint16_t PACKET_HEADER_SIZE = sizeof(uint16_t) + sizeof(unsigned short);

	ClientNetworkIO(asio::ip::tcp::socket&& socket);

	~ClientNetworkIO() { };

	void Drop() { } ;

	void MakeSocketAsyncReadSome();

	void SocketAsyncReadSome(const asio::error_code& ec, size_t size);

	void SocketAsyncWriteSome() { } ;

	void SetEncryptionKey(uint32_t& key) { };

	bool HasEncryptionKey() const
	{
		return m_hasEncryptionKey;
	};

	std::string GetPeerAddress() const
	{
		return m_socket.remote_endpoint().address().to_string();
	};

public:
	fwEvent<uint8_t*, unsigned short&> OnSocketReadPacket;

private:

	asio::ip::tcp::socket m_socket;

	Lunia::Net::Crypt::Box m_decryptor; /* TODO Should probably be a singleton */

	uint8_t m_lastReceivedBuffer[READ_BUFFER_LENGTH] = { };

	bool m_hasEncryptionKey = false;
};
