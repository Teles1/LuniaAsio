#include "PacketHead.h"

void PacketHeadFromClient::Serialize(Lunia::Serializer::IStreamWriter& out)
{
	out.Begin(L"Head"); /* Can be set automatically before sending to the client */
	out.Write(L"Result", static_cast<const int>(Result));
	out.Write(L"ServerTime", ServerTime);
	out.Write(L"UserIP", UserIP);
	out.Write(L"EncryptKey", EncryptKey);
};

void PacketHeadFromClient::Deserialize(Lunia::Serializer::IStreamReader& out)
{
	out.Begin(L"Head"); /* Can bet set automatically when reading from the buffer*/
	out.Read(L"ProtocolVersion", ProtocolVersion);
	out.Read(L"Checksums", Checksums);
};

static utils::InitFunction InitFunction([]()
{
	g_gameServer->Proxy->PacketHandler.Connect([](std::shared_ptr<ClientProxyLobby>& client, PacketHeadFromClient& packet)
	{
		std::cout << "Handling `Head` packet, from client" << std::endl;
	});
});