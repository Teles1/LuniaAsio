#include "ClientNetworkIO.h"

ClientNetworkIO::ClientNetworkIO(asio::ip::tcp::socket&& socket) 
	: m_socket(std::move(socket)) /* WARN socket is moved a shit ton of times here */
	, m_decryptor(false)
{
	MakeSocketAsyncReadSome();
};

void ClientNetworkIO::MakeSocketAsyncReadSome()
{
	m_socket.async_read_some(asio::buffer(m_buffer, ClientNetworkIO::READ_BUFFER_LENGTH),
		std::bind(&ClientNetworkIO::SocketAsyncReadSome, this, std::placeholders::_1, std::placeholders::_2));
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

		/*
			for (int i = 0; i < size; i++)
			{
				printf("%02hhX ", m_buffer[i]);

				if (i != 0 && i % 12 == 0)
					std::cout << std::endl;
			}

			std::cout << std::endl;
			std::cout << std::endl;
		*/

		size_t bytesRead = 0;

		while (bytesRead < size)
		{
			if (size - bytesRead >= ClientNetworkIO::PACKET_HEADER_SIZE) /* We are still reading the Header */
			{
				if (HasEncryptionKey())
					m_decryptor.Translate(m_buffer, size);

				unsigned short * length = reinterpret_cast<unsigned short *>(&m_buffer[bytesRead]);

				unsigned short * maybeNetStream = reinterpret_cast<unsigned short*>(&m_buffer[bytesRead + 2]);

				bool isNetStream = *maybeNetStream == NETSTREAM_HASHED;

				if (isNetStream)
				{
					unsigned short lengthNoHeader = *length;

					if (lengthNoHeader > size)
					{
						/* Dunno, packet is fucked */
					}
					else
					{
						char * bufferNoHeader = &m_buffer[bytesRead];

						bytesRead += *length;

						OnSocketReadPacket(bufferNoHeader, lengthNoHeader); 

						MakeSocketAsyncReadSome();
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
		// std::cout << "ClientNetworkIO " << ec << std::endl;
	}
};

void ClientNetworkIO::MakeSocketAsyncWriteSome(uint8_t * buffer, size_t size)
{
	m_socket.async_send(asio::buffer(buffer, size),
		std::bind(&ClientNetworkIO::SocketAsyncWriteSome, this, std::placeholders::_1, std::placeholders::_2));
};

void ClientNetworkIO::SocketAsyncWriteSome(const asio::error_code& ec, size_t size)
{
	// std::cout << "ClientNetworkIO::SocketAsyncWriteSome | " << ec << std::endl;
};

void ClientNetworkIO::Drop() 
{
	m_socket.release();

	m_socket.close();
};

void ClientNetworkIO::SetEncryptionKey(uint32_t& key)
{
	this->m_decryptor.SetKey(key);

	m_hasEncryptionKey = true;
};

bool ClientNetworkIO::HasEncryptionKey() const
{
	return this->m_hasEncryptionKey;
};


std::string ClientNetworkIO::GetPeerAddress() const
{
	// return "127.0.0.1"; 
	return this->m_socket.remote_endpoint().address().to_string();
};