#ifndef ClientNetworkIO_H
#define ClientNetworkIO_H
#pragma once

#include "../NetworkIO.h"
#include <Network/Crypt/Crypt.h>
namespace Lunia{
	struct ClientNetworkIO : std::enable_shared_from_this<ClientNetworkIO> /* TODO Is it necessary? */
	{
		static const uint16_t READ_BUFFER_LENGTH = 2 << 12;
		static const uint16_t PACKET_HEADER_SIZE = sizeof(uint16_t) + sizeof(unsigned short);
		static const uint16_t NETSTREAM_HASHED   = (uint16_t) 0x55E0;

		ClientNetworkIO(asio::ip::tcp::socket&& socket);

		~ClientNetworkIO()
		{

		};

	private:
		void MakeSocketAsyncReadSome();

		void SocketAsyncReadSome(const asio::error_code& ec, size_t size);

		void SocketAsyncWriteSome(const asio::error_code& ec, size_t size);

	public:
		void MakeSocketAsyncWriteSome(uint8_t * buffer, size_t size);

		void Drop();

		void SetEncryptionKey(uint32_t& key);

		bool HasEncryptionKey() const;

		std::string GetPeerAddress() const;

	public:
		fwEvent<char*, unsigned short&> OnSocketReadPacket;

	private:

		asio::ip::tcp::socket m_socket;

		Lunia::Net::Crypt::Box m_decryptor; /* TODO Should probably be a singleton */

		char m_buffer[READ_BUFFER_LENGTH];

		bool m_hasEncryptionKey = false;
	};
}
#endif // ! ClientNetworkIO_H