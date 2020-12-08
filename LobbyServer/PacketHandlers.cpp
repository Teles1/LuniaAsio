#include "fwPacketListener.h"
#include "../Core/Utils/InitFunction.h"

static utils::InitFunction init([]() 
{
	fwPacketListener::GetInstance()->Connect([](Lobby::UserSharedPtr& user, Lobby::Protocol::Head& packet)
	{
		INFO_LOG("fwPacketListener :: userId@{0} :: protocol@Head", user->GetUserId());

		packet.Checksums[0] = 1;

		packet.Result = Lobby::Protocol::Head::Results::Ok;

		packet.EncryptKey = 0;
		packet.ServerTime = DateTime::Now();
		packet.UserIP = "127.0.0.1";

		StaticBuffer< 4096 > buffer;
		try
		{
			Serializer::StreamWriter writer(buffer);
			writer.Write(packet);
		}
		catch (Exception&) // catch something like buffer overflow
		{
			//ERROR_LOG("unable to send", 0);
			return;
		}

		user->SendAsync(reinterpret_cast<uint8*>(buffer.GetData()), buffer.GetLength());
	});

	fwPacketListener::GetInstance()->Connect([](Lobby::UserSharedPtr& user, Lobby::Protocol::Auth& packet)
	{
		INFO_LOG("fwPacketListener :: userId@{0} :: protocol@Auth", user->GetUserId());
	});
});