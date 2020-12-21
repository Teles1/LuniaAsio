#include "ClientNetworkIO.h"

ClientNetworkIO::ClientNetworkIO(asio::ip::tcp::socket&& socket) :
	m_socket(std::move(socket)) /* WARN socket is moved a shit ton of times here */
	, m_decryptor(false)
{
	MakeSocketAsyncReadSome();
};

void ClientNetworkIO::MakeSocketAsyncReadSome()
{
	m_socket.async_read_some(asio::buffer(m_buffer, READ_BUFFER_LENGTH),
		std::bind(&ClientNetworkIO::SocketAsyncReadSome, this/* shared_from_this() */, std::placeholders::_1, std::placeholders::_2));
}

void ClientNetworkIO::SocketAsyncReadSome(const asio::error_code& ec, size_t size)
{
	if (!ec)
	{

		/*
		Header:
			Size
			NetStream
			Hash
		*/

		size_t bytesRead = 0;

		while (bytesRead < size)
		{
			if (size - bytesRead >= PACKET_HEADER_SIZE) /* We are still reading the Header */
			{
				uint32_t encryptionKey = m_decryptor.GetKey(); /* TODO Should probably a reference */

				if (m_hasEncryptionKey)
					m_decryptor.Translate(&m_buffer[bytesRead], PACKET_HEADER_SIZE);

				unsigned short * length = reinterpret_cast<unsigned short *>(&m_buffer[bytesRead]);

				unsigned short * maybeNetStream = reinterpret_cast<unsigned short*>(&m_buffer[bytesRead + 2]);

				bool isNetStream = *maybeNetStream == NETSTREAM_HASHED;

				if (isNetStream)
				{
					unsigned short lengthNoHeader = *length - PACKET_HEADER_SIZE;

					if (lengthNoHeader > size)
					{
						/* Dunno, packet is fucked */
					}
					else
					{
						char * bufferNoHeader = &m_buffer[bytesRead + PACKET_HEADER_SIZE];

						if (m_hasEncryptionKey)
							m_decryptor.Translate(bufferNoHeader, lengthNoHeader);

						// unsigned short * packetNameHashed = reinterpret_cast<unsigned short*>(bufferNoHeader);

						// std::cout << "packetNameHashed " << *packetNameHashed << std::endl;

						/*
							WARN packetNameHashed might be +1 for some reason ?! 
						*/

						bytesRead += *length;

						OnSocketReadPacket(bufferNoHeader, lengthNoHeader); 
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

		MakeSocketAsyncReadSome();
	}
	else
	{
		std::cout << "ErroCode " << ec << std::endl;
	}
};

void ClientNetworkIO::SocketAsyncWriteSome()
{

};