#include "fwPacketListener.h"
#include "../Core/Utils/InitFunction.h"
#include <Core/Utils/Math/Random.h>
#include "./Network/Api/Api.h"
static utils::InitFunction init([]() 
{
	fwPacketListener::GetInstance()->Connect(
		[](Lobby::UserSharedPtr& user, Lobby::Protocol::Head& packet)
		{
			Logger::GetInstance()->Info("fwPacketListener :: userId@{0} :: protocol@Head", user->GetUserId());
			Lobby::Protocol::Head sendPacket;
			if (packet.ProtocolVersion == Constants::Version)
				sendPacket.Result = Lobby::Protocol::Head::Results::Ok;
			else
				sendPacket.Result = Lobby::Protocol::Head::Results::VersionMismatched;

			sendPacket.EncryptKey = Math::Random();
			user->SetCryptoKey(sendPacket.EncryptKey);
			sendPacket.ServerTime = DateTime::Now();
			sendPacket.UserIP = user->GetPeerAddress();
			user->Send(sendPacket);
			user->QueryAliveAuth();
		});	

	fwPacketListener::GetInstance()->Connect(
		[](Lobby::UserSharedPtr& user, Lobby::Protocol::Auth& packet)
		{
			Logger::GetInstance()->Info("fwPacketListener :: userId@{0} :: protocol@Auth", user->GetUserId());
			user->SetUserLocale(packet.Locale);
			Api api("CheckAccount");
			api << packet.AccountId;
			api << packet.EncryptedPassword;
			api << user->GetPeerAddress();
			if (auto response = api.Send()){
				
			}
		});
});