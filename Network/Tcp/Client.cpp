#include "Client.h"
#include <Network/NetStream.h>
namespace Lunia {
	namespace XRated {
		namespace Net {
			ClientTcp::ClientTcp(tcp::socket&& s)
				: m_socket(std::move(s)) //ClientTcp now Owns the socket.
				, m_decryptor(false)
				, m_isEncryptKey(false)
				, m_receiveBuffer(2 << 14)
			{
				std::cout << "ClientTcp was created" << std::endl;
			}

			void ClientTcp::SendAsync(uint8* data, size_t size) {
				m_socket.async_send(asio::buffer(data, size),
					std::bind(&ClientTcp::WroteSome, this, std::placeholders::_1, std::placeholders::_2));
			}

			void ClientTcp::HandleRead() {
				m_socket.async_read_some(asio::buffer(m_buffer, READ_DATA_BUFFER_LENGTH),
					std::bind(&ClientTcp::ReceivedSome, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
			}

			void ClientTcp::CloseSocket() {
				m_socket.close();
				m_socket.release();
			}

			ClientTcp::~ClientTcp() {
				std::cout << "ClientTcp was destroyed" << std::endl;
			}

			void ClientTcp::ReceivedSome(const error_code& ec, size_t lenght)
			{
				if (!ec)
				{
					if (m_receiveBuffer.GetLength() >= m_receiveBuffer.GetMaxLength()) {
						LoggerInstance().Error("Buffer Overflow on ClientTcp");
						CloseSocket();
						return;
					}
					int check = m_receiveBuffer.Append(&m_buffer[0], (int)lenght);
					if (check != lenght) {
						LoggerInstance().Error("bufferOverflow ClientTcp");
					}
					int processed = OnReceived((uint8*)m_receiveBuffer.GetData(), m_receiveBuffer.GetLength());
					m_receiveBuffer.Remove(0, processed);
					HandleRead();
				}
			}

			int ClientTcp::OnReceived(uint8* buffer, size_t lenght)
			{
				//try {
				int total = 0;
				int result = 0;
				do {
					if (lenght - total > sizeof(LengthType)) {
						uint8* work = buffer + total;
						LengthType* work_length = (LengthType*)work;

						LengthType backup = *work_length;
						uint32 key = m_decryptor.GetKey();

						m_decryptor.Translate(work, sizeof(LengthType));
						LengthType size = *work_length;
						if (size < sizeof(LengthType))
							LoggerInstance().Exception("invalid size({}) set", lenght);
						if (size > lenght - total) {
							//rollback
							m_decryptor.SetKey(key);
							*work_length = backup;
							break;
						}

						m_decryptor.Translate(work + sizeof(LengthType), size - sizeof(LengthType));
						result = Parse(work, lenght - total);
						total += result;
						if (total >= lenght)
							break;
					}
				} while (result);
				return total;
			}

			void ClientTcp::SetCryptoKey(uint32& newKey)
			{
				m_decryptor.SetKey(newKey);
				m_isEncryptKey = true;
			}

			uint32 ClientTcp::GetCryptoKey()
			{
				return m_decryptor.GetKey();
			}

			const std::string ClientTcp::GetPeerAddress() const {
				return std::move(m_socket.remote_endpoint().address().to_string());
			}

			void ClientTcp::WroteSome(const error_code& error, size_t size)
			{
			}
		}
	}
}