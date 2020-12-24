#include "ClientProxy.h"

#include "Network/NetStream.h"

ClientProxy::ClientProxy(asio::ip::tcp::socket&& socket) : ClientNetworkIO(std::move(socket)) /* WARN socket is moved a shit ton of times here */
{
	std::cout << "ClientProxy : constructor called" << std::endl;
};

void ClientProxy::MakeSocketAsyncWriteSerializable(Lunia::Serializer::ISerializable& packet)
{
	Lunia::StaticBuffer<2 << 12> buffer;

	Lunia::Net::StreamWriter streamWriter(buffer);

	// unsigned short * packetNameHashed = (char*)streamWriter.buffer.GetData();

	try
	{
		streamWriter.Write(packet);

		// std::cout << "server -> Client@" << this->GetId() << " 0x" << packetNameHashed << std::endl;
	}
	catch (Lunia::Exception& e)
	{
		Logger::GetInstance().Error("MakeSocketAsyncWriteSerializable:: Client@{0} couldn't parse packet", this->GetId());
		return;
	}

	this->MakeSocketAsyncWriteSome(buffer.GetData(), buffer.GetLength());
}

uint32_t ClientProxy::GetId() const
{
	return this->m_id;
}

void ClientProxy::SetId(const uint32_t& id)
{
	this->m_id = id;
}

void ClientProxy::SetLocale(const std::wstring& locale)
{
	this->m_locale = locale;
}

void ClientProxy::SetActiveAccountName(const std::wstring& accountName)
{
	this->m_activeAccountName = accountName;
}

std::wstring ClientProxy::GetActiveAccountName() const
{
	return this->m_activeAccountName;
}

bool ClientProxy::IsWaitingOnPing() const
{
	return this->m_isWaitingOnPing;
}