#include "fwPacketListener.h"
#include "../Core/Utils/InitFunction.h"
#include <Core/Utils/Math/Random.h>
#include "./Network/Api/Api.h"
#include "UserRegistry.h"

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
			const Answer result = api.RequestApi();
			Lobby::Protocol::Auth sendPacket;

			sendPacket.Result = static_cast<Lobby::Protocol::Auth::Results>(result.errorCode);

			switch (sendPacket.Result)
			{
			case Lobby::Protocol::Auth::Results::Ok:

				user->SetUserAccountName(packet.AccountId); //It's fine because we already know that the account exists and the user is authenticated

				sendPacket.AccountId = std::move(packet.AccountId);

				net::UserRegistry::GetInstance()->AuthenticateUser(user);

				break;
			default:
				net::UserRegistry::GetInstance()->RemoveUser(user);
				break;
			}

			user->Send(sendPacket);
		});
});