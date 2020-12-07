#include "Client.h"
using namespace net;

ClientTcp::ClientTcp(tcp::socket&& s)
	: m_socket(std::move(s)) //ClientTcp now Owns the socket.
	, m_decryptor(false)
	, m_isEncryptKey(false)
{
	std::cout << "ClientTcp was created" << std::endl;
}

void net::ClientTcp::SendAsync(uint8* data, size_t& size){
	m_socket.async_send(boost::asio::buffer(data, size),
		std::bind(&ClientTcp::WroteSome, this, std::placeholders::_1, std::placeholders::_2));
}

void net::ClientTcp::HandleRead() {
	m_socket.async_read_some(ba::buffer(m_buffer, READ_DATA_BUFFER_LENGTH),
		std::bind(&ClientTcp::ReceivedSome, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
}

ClientTcp::~ClientTcp() {
	std::cout << "ClientTcp was destroyed" << std::endl;
}

void ClientTcp::ReceivedSome(const error_code& ec, size_t size)
{
	if (!ec)
	{
		auto t = std::thread([=]() {
			try {
				int total = 0;
				uint8* work = new uint8[Constants::HeaderSize]{ 0 };
				do {
					if (size - total >= Constants::HeaderSize) {
						if (m_isEncryptKey) {
							uint32 keyBackup = GetCryptoKey();
							//INFO_LOG("Work[{0}]", StringUtil::GetString(work, Serializer::HeaderSize));
							memcpy(work, m_buffer + total, Constants::HeaderSize);
							//INFO_LOG("Work[{0}]", StringUtil::GetString(work, Serializer::HeaderSize));
							m_decryptor.Translate(work, Constants::HeaderSize); // translate the header only

							LengthType* work_length = reinterpret_cast<LengthType*>(work);
							LengthType pSize = *work_length;

							HashType* header_length = reinterpret_cast<HashType*>(work + 2);
							HashType header = *header_length;
							//printf("Psize[%.2X], Hash[%.2X]\n", pSize, header);
							if (header == Constants::NetStreamHash) {
								//Translate only the bytes related to the packet
								if (pSize - Constants::HeaderSize > size) {
									//Back up the packet for the next iteration. There is not enough bytes to parse this data, or, the client is trolling?
									WARN_LOG("Could not read entire packet. We should back up the packet for the next iteration WARN!!!!! pSize[{0}] > size[{1}]", pSize - Constants::HeaderSize, size);
								}
								else {
									//Packet can be deserialized safely.
									//Packet can be fully read because lenght > pSize
									m_decryptor.Translate(&m_buffer[size_t(total) + size_t(Constants::HeaderSize)], pSize - Constants::HeaderSize); // Decryption done
									Parse(&m_buffer[size_t(total) + size_t(Constants::HeaderSize)], pSize - Constants::HeaderSize);
									total += pSize;
								}
								//size - total - HeaderSize > lenght  means that there is more data to be processed.
								// Eventually we gonna have to add a system to simple add data to a buffer or maybe a pointer to hold the data?
							}
							else {
								SetCryptyoKey(keyBackup);
								total += 1;
							}
						}
						else { // If there is no cryptography we keep on adding 1 to the buffer until we find a NetStream
							LengthType* work_length = reinterpret_cast<LengthType*>(m_buffer + total);
							LengthType pSize = *work_length;

							HashType* header_length = reinterpret_cast<HashType*>(m_buffer + total + 2);
							HashType header = *header_length;
							//printf("NonCrypt Psize[%.2X], Hash[%.2X]\n", pSize, header);
							if (header == Constants::NetStreamHash) {
								//printf("size[%d] total[%d] pSize[%d]\n", size, total, pSize);
								Parse(&m_buffer[size_t(total) + size_t(Constants::HeaderSize)], pSize - Constants::HeaderSize);
								total = +pSize;
							}
							else
								total += 1;
						}
						/*
						if (size - total < Serializer::HeaderSize) //Read Enough
							break;
						*/
					}
					else //Cant read header.
						break;
					/*
					else {
						WARN_LOG("Cannot read beyond stream");
						for (int i = 0; i < size; i++) {
							printf("%02x ", src[i]);
						}
						printf("\n");
						break;
					}*/
					if (total >= size)
						break;
				} while (true);
				//Cleaning up my pointer.
				delete[] work;
			}
			catch (...) {
				std::cout << "Exception trown\n";
			}

			});

		t.detach();
	}
}

void ClientTcp::SetCryptyoKey(uint32& newKey)
{
	m_decryptor.SetKey(newKey);
	m_isEncryptKey = true;
}

uint32 ClientTcp::GetCryptoKey()
{
	return m_decryptor.GetKey();
}

void ClientTcp::WroteSome(const error_code& error, size_t size)
{
}
