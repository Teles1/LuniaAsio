#include "ClientNetworkIO.h"

ClientNetworkIO::ClientNetworkIO(asio::ip::tcp::socket&& socket) :
	m_socket(std::move(socket)) /* WARN socket is moved a shit ton of times here */
	, m_decryptor(false)
{
	MakeSocketAsyncReadSome();
};

void ClientNetworkIO::MakeSocketAsyncReadSome()
{
	m_socket.async_read_some(asio::buffer(m_lastReceivedBuffer, READ_BUFFER_LENGTH),
		std::bind(&ClientNetworkIO::SocketAsyncReadSome, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
}

void ClientNetworkIO::SocketAsyncReadSome(const asio::error_code& ec, size_t size)
{
	if (!ec)
	{

		int bytesRead = 0;

		uint8_t buffer[READ_BUFFER_LENGTH] = { 0 };                        /* TODO Should we copy it or just read from the original? */
		memcpy(buffer, m_lastReceivedBuffer, sizeof(m_lastReceivedBuffer)); /* TODO Should we copy it or just read from the original? */

		// uint8_t 

		while (bytesRead < size)
		{

			if (size - bytesRead >= PACKET_HEADER_SIZE) /* We are still reading the Header */
			{
				/*
				Header:
					Size
					NetStream
					Hash
				*/

				uint32_t encryptionKey = m_decryptor.GetKey(); /* TODO Should probably a reference */

				if (m_hasEncryptionKey)
					m_decryptor.Translate(buffer + bytesRead, PACKET_HEADER_SIZE);

				unsigned short length = (unsigned short)*buffer + bytesRead;

				uint16_t isNetStream = ((uint16_t)(*buffer + bytesRead + 2)) == (uint16_t)0x55E0; /* hash::NetStream */

				if (isNetStream)
				{

					if (length - PACKET_HEADER_SIZE > size)
					{
						/* Dunno, packet is fucked */
					}
					else
					{
						if (m_hasEncryptionKey)
							m_decryptor.Translate(&buffer[bytesRead + PACKET_HEADER_SIZE], length - PACKET_HEADER_SIZE);

						OnSocketReadPacket(&buffer[bytesRead], length);

						bytesRead += length;
					}
				}
				else
				{
					bytesRead++;
				}
			}
			else
			{
				break;
			}
		}
	}
	else
	{
		std::cout << "ErroCode " << ec << std::endl;
	}
};