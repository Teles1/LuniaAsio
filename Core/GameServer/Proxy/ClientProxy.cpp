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

	try
	{
		streamWriter.Write(packet);
	}
	catch (Lunia::Exception& e)
	{
		Logger::GetInstance().Error("User@{0} Unable to Parse packet", this->GetId());
		return;
	}

	this->MakeSocketAsyncWriteSome(buffer.GetData(), buffer.GetLength());
}