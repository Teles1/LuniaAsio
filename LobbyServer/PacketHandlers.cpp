#include "fwPacketListener.h"
#include <Core/Utils/InitFunction.h>
#include <Core/Utils/Math/Random.h>
#include <LobbyServer/Common.h>
#include <Core/Utils/ThreadPool.h>
#include <Core/Utils/ConfigReader.h>
#include <LobbyServer/LobbyServer.h>

namespace Lunia {
	namespace XRated {
		namespace Lobby {
			static utils::InitFunction init([]()
				{
					fwPacketListener::GetInstance().Connect(
						[](Lobby::UserSharedPtr& user, Protocol::ToServer::Head& packet)
						{
							Logger::GetInstance().Info("fwPacketListener :: userId@{0} :: protocol@Head", user->GetId());
							Lobby::Protocol::FromServer::Head sendPacket;
							if (packet.ProtocolVersion == Lunia::Constants::Version)
								sendPacket.Result = Lobby::Protocol::FromServer::Head::Results::Ok;
							else
								sendPacket.Result = Lobby::Protocol::FromServer::Head::Results::VersionMismatched;

							sendPacket.EncryptKey = Math::Random();
							user->SetCryptoKey(sendPacket.EncryptKey);
							sendPacket.ServerTime = DateTime::Now();
							sendPacket.UserIP = user->GetPeerAddress();
							user->Send(sendPacket);
							user->SatisfyAlivePingWait();
						});
					fwPacketListener::GetInstance().Connect(
						[](Lobby::UserSharedPtr& user, Lobby::Protocol::ToServer::Auth& packet)
						{
							Logger::GetInstance().Info("fwPacketListener :: userId@{0} :: protocol@Auth", user->GetId());
							Http::TextPacket request("Auth_Publisher");
							request << packet.AccountId;
							request << packet.EncryptedPassword;
							request << user->GetPeerAddress();
							//UserRegistry().AuthenticateUser(user);//After Second password.
							user->SetLocale(packet.Locale);
							user->SetAccountName(packet.AccountId);
							DB().Request(user, request, *(user.get()), &User::Authorize, true);
						});
					fwPacketListener::GetInstance().Connect(
						[](Lobby::UserSharedPtr& user, Lobby::Protocol::ToServer::CheckSecondPassword& packet)
						{
							Logger::GetInstance().Info("fwPacketListener :: userId@{0} :: protocol@CheckSecondPassword", user->GetId());
							Http::TextPacket request("Auth_2nd_Check");
							request << user->GetAccountName();
							DB().Request(user, request, *(user.get()), &User::Auth_2nd_Check, true);
						});
					//fwPacketListener::GetInstance().Connect(
					//	[](Lobby::UserSharedPtr& user, Lobby::Protocol::CreateSecondPassword& packet)
					//	{
					//		Logger::GetInstance().Info("fwPacketListener :: userId@{0} :: protocol@CreateSecondPassword", user->GetId());
					//		Net::Api api("Auth_2nd_Create");
					//		api << user->GetAccountName() << packet.Password;
					//		const Net::Answer result = api.RequestApi();
					//		if (result.errorCode == 0) {
					//			Lobby::Protocol::SecondPasswordCreated sendPacket;
					//			sendPacket.Result = static_cast<Lobby::Protocol::SecondPasswordCreated::Results>(result.errorCode);
					//			user->Send(sendPacket);
					//		}
					//	});
					//fwPacketListener::GetInstance().Connect(
					//	[](Lobby::UserSharedPtr& user, Lobby::Protocol::DeleteSecondPassword& packet)
					//	{
					//		Logger::GetInstance().Info("fwPacketListener :: userId@{0} :: protocol@DeleteSecondPassword", user->GetId());
					//		Net::Api api("Auth_2nd_Delete");
					//		api << user->GetAccountName() << packet.Password;
					//		const Net::Answer result = api.RequestApi();
					//		if (result.errorCode == 0) {
					//			Lobby::Protocol::SecondPasswordDeleted sendPacket;
					//			sendPacket.Result = static_cast<Lobby::Protocol::SecondPasswordDeleted::Results>(result.errorCode);
					//			user->Send(sendPacket);
					//		}

					//	});
					//fwPacketListener::GetInstance().Connect(
					//	[](Lobby::UserSharedPtr& user, Lobby::Protocol::ModifySecondPassword& packet)
					//	{
					//		Logger::GetInstance().Info("fwPacketListener :: userId@{0} :: protocol@ModifySecondPassword", user->GetId());
					//		Net::Api api("Auth_2nd_Modify");
					//		api << user->GetAccountName() << packet.Password << packet.NewPassword;
					//		const Net::Answer result = api.RequestApi();
					//		if (result.errorCode == 0) {
					//			Lobby::Protocol::SecondPasswordModified sendPacket;
					//			sendPacket.Result = static_cast<Lobby::Protocol::SecondPasswordModified::Results>(result.errorCode);
					//			user->Send(sendPacket);
					//		}

					//	});
					//fwPacketListener::GetInstance().Connect(
					//	[](Lobby::UserSharedPtr& user, Lobby::Protocol::SignInSecondPassword& packet)
					//	{
					//		Logger::GetInstance().Info("fwPacketListener :: userId@{0} :: protocol@SignInSecondPassword", user->GetId());
					//		Net::Api api("Auth_2nd_Sign");
					//		api << user->GetAccountName() << packet.Password;
					//		const Net::Answer result = api.RequestApi();

					//		Lobby::Protocol::SecondPasswordSignedIn sendPacket;

					//		sendPacket.Result = static_cast<Lobby::Protocol::SecondPasswordSignedIn::Results>(result.errorCode);

					//		user->Send(sendPacket);

					//	});
					//fwPacketListener::GetInstance().Connect(
					//	[](Lobby::UserSharedPtr& user, Lobby::Protocol::Terminate& packet)
					//	{
					//		Logger::GetInstance().Info("fwPacketListener :: userId@{0} :: protocol@Terminate", user->GetId());
					//		//Gotta safely disconect the user. 
					//		Net::Api api("Terminate");
					//		api << user->GetAccountName();
					//		const Net::Answer result = api.RequestApi();
					//		Lobby::Protocol::Terminate sendPacket;
					//		sendPacket.Result = Lobby::Protocol::Terminate::Results::Ok;
					//		user->Send(sendPacket);
					//		UserRegistry().RemoveUser(user);
					//	});
					//fwPacketListener::GetInstance().Connect(
					//	[](Lobby::UserSharedPtr& user, Lobby::Protocol::CreateCharacter& packet)
					//	{
					//		Logger::GetInstance().Info("fwPacketListener :: userId@{0} :: protocol@CreateCharacter", user->GetId());

					//		bool checkLicense = true;
					//		if (XRated::Constants::IsDefaultClass(packet.ClassType) == false)
					//			checkLicense = user->DoesHaveLicense(packet.ClassType);

					//		Net::Api api("CreateCharacter");
					//		api << user->GetAccountName() << packet.Name << static_cast<int>(packet.ClassType) << checkLicense;
					//		const Net::Answer result = api.RequestApi();
					//		Lobby::Protocol::CreateCharacter sendPacket;
					//		sendPacket.Result = static_cast<Lobby::Protocol::CreateCharacter::Results>(result.errorCode);
					//		if (result.errorCode == 0) {
					//			sendPacket.CharacterInfo.CharacterName = StringUtil::ToUnicode(result.resultObject["characterName"].get<std::string>());
					//			sendPacket.CharacterInfo.CharacterSerial = result.resultObject["id"].get<int64>();
					//			sendPacket.CharacterInfo.VirtualIdCode = result.resultObject["id"].get<uint32>();
					//			sendPacket.CharacterInfo.ClassType = static_cast<XRated::Constants::ClassType>(result.resultObject["classNumber"].get<int>());
					//			sendPacket.CharacterInfo.Level = result.resultObject["stageLevel"].get<uint16>();
					//			sendPacket.CharacterInfo.Exp = result.resultObject["stageExp"].get<uint32>();
					//			sendPacket.CharacterInfo.PvpLevel = result.resultObject["pvpLevel"].get<uint16>();
					//			sendPacket.CharacterInfo.PvpExp = result.resultObject["pvpExp"].get<uint32>();
					//			sendPacket.CharacterInfo.WarLevel = result.resultObject["warLevel"].get<uint16>();
					//			sendPacket.CharacterInfo.WarExp = result.resultObject["warExp"].get<uint32>();
					//			sendPacket.CharacterInfo.StateFlags = static_cast<XRated::CharacterStateFlags>(result.resultObject["instantStateFlag"].get<int>());
					//			sendPacket.CharacterInfo.RebirthCount = result.resultObject["characterRebirth"]["rebirthCount"].get<uint16>();
					//			sendPacket.CharacterInfo.StoredLevel = result.resultObject["characterRebirth"]["storedLevel"].get<uint16>();

					//			for (auto& y : result.resultObject["stageLicenses"].get<json>()) { //[{"stageHash":19999,"accessLevel":1,"difficulty": 1}]
					//				sendPacket.CharacterInfo.Licenses.push_back(XRated::StageLicense(y["stageGroupHash"].get<uint32>(), y["accessLevel"].get<uint16>()));
					//			}

					//			for (auto& y : result.resultObject["items"].get<json>()) {
					//				XRated::ItemSlot slot;
					//				slot.Position.Bag = y["bagNumber"].get<uint8>(); // equipment slots at
					//				slot.Position.Position = y["positionNumber"].get<uint8>();
					//				slot.Stacked = 1; // equipments cannot be stacked
					//				slot.Id = y["itemHash"].get<uint32>();
					//				slot.InstanceEx.Instance = y["instance"].get<int64>();
					//				slot.InstanceEx.ExpireDate.Parse(y["itemExpire"].get<std::wstring>());
					//				sendPacket.CharacterInfo.Equipments.push_back(slot);
					//			}
					//			user->m_Characters.push_back(sendPacket.CharacterInfo);
					//		}
					//		user->Send(sendPacket);
					//	});
					//fwPacketListener::GetInstance().Connect(
					//	[](Lobby::UserSharedPtr& user, Lobby::Protocol::DeleteCharacter& packet)
					//	{
					//		Logger::GetInstance().Info("fwPacketListener :: userId@{0} :: protocol@DeleteCharacter", user->GetId());

					//		if (!user->IsAccountAuthorized()) {
					//			Logger::GetInstance().Error(L"Non authorized user trying to delete a AccountName: {0}, characterName: {1}", user->GetAccountName(), packet.Name);
					//			UserRegistry().RemoveUser(user);
					//			return;
					//		}
					//		if (!user->IsAValidCharacterName(packet.Name)) {
					//			Logger::GetInstance().Error(L"User is trying to delete a characterName that does not exist. AccountName: {0}, characterName: {1}", user->GetAccountName(), packet.Name);
					//			UserRegistry().RemoveUser(user);
					//			return;
					//		}
					//		Net::Api api("DeleteCharacter");
					//		api << packet.Name;
					//		const Net::Answer result = api.RequestApi();
					//		Lobby::Protocol::DeleteCharacter sendPacket;
					//		sendPacket.Result = static_cast<Lobby::Protocol::DeleteCharacter::Results>(result.errorCode);
					//		if (result.errorCode == 0) {
					//			sendPacket.DeletedCharacter = packet.Name;
					//			if (!user->DeleteCharacter(packet.Name)) {
					//				Logger::GetInstance().Error(L"Could not Delete the specified characterName AccountName: {0}, characterName: {1}", user->GetAccountName(), packet.Name);
					//				UserRegistry().RemoveUser(user);
					//			}
					//		}
					//		user->Send(sendPacket);
					//	});
					//fwPacketListener::GetInstance().Connect(
					//	[](Lobby::UserSharedPtr& user, Lobby::Protocol::SaveKeySetting& packet)
					//	{
					//		Logger::GetInstance().Info("fwPacketListener :: userId@{0} :: protocol@SaveKeySetting", user->GetId());

					//		Net::Api api("SaveKeySetting");
					//		api << packet.Account;
					//		const Net::Answer result = api.RequestPost(StringUtil::ToASCII(packet.Keycodes));
					//		if (result.errorCode != 0) {
					//			//key save error;
					//			return;
					//		}
					//	});
					//fwPacketListener::GetInstance().Connect(
					//	[](Lobby::UserSharedPtr& user, Lobby::Protocol::Alive& packet)
					//	{
					//		Logger::GetInstance().Info("fwPacketListener :: userId@{0} :: protocol@Alive", user->GetId());
					//		Lobby::Alive::AliveData answer(packet.Index, packet.Value1, packet.Value2, packet.Value3);

					//		user->SetAliveAsLastTickAlivePing(answer);
					//	});
					//fwPacketListener::GetInstance().Connect(
					//	[](Lobby::UserSharedPtr& user, Lobby::Protocol::SelectCharacter& packet)
					//	{
					//		Logger::GetInstance().Info("fwPacketListener :: userId@{0} :: protocol@SelectCharacter", user->GetId());
					//		Net::Api api("SelectCharacter");
					//		//api << Config::GetInstance().m_ServerName;
					//		api << user->GetAccountName();
					//		api << packet.CharacterName;
					//		auto result = api.RequestApi();
					//		Lobby::Protocol::SelectCharacter sendPacket;
					//		sendPacket.Result = static_cast<Lobby::Protocol::SelectCharacter::Results>(result.errorCode);
					//		if (result.errorCode == 0 && user->IsAValidCharacterName(packet.CharacterName) && user->SetSelectedCharacter(packet.CharacterName)) {
					//			sendPacket.SelectedCharacter = StringUtil::ToUnicode(result.resultObject["characterName"].get<std::string>());
					//			sendPacket.CharacterStates = user->GetCharacterStateFlags();
					//			sendPacket.CharacterStates.IsPcRoom = 1; // AllM also sets this for some reason.
					//			sendPacket.CharacterStates.IsAdmin = 1;
					//			//after checking if the user is admin or not we gonna update the flags.
					//			user->SetCharacterStateFlags(sendPacket.CharacterStates);
					//			//Family check if there is a family to be joined as a guest will come in here

					//			//aswell as making sure to send the achievement server to the client in here.
					//			{
					//				Net::Api achivementApi("FindAchievementServer");
					//				achivementApi << packet.CharacterName;
					//				/*achivementApi.GetAsync(
					//					[&](Net::Answer& result) {
					//						Lobby::Protocol::AchievementServerAssigned packetAchievement;
					//						packetAchievement.Result = Lobby::Protocol::AchievementServerAssigned::Result::Ok;
					//						if (result.errorCode == 0) {
					//							if (user->IsAValidCharacterName(packet.CharacterName) && !result.resultObject.is_null()) {
					//								packetAchievement.ServerName = StringUtil::ToUnicode(result.resultObject["serverName"].get<std::string>());
					//								packetAchievement.serverAddress.Ip = result.resultObject["clientIp"].get<std::string>();
					//								packetAchievement.serverAddress.Port = result.resultObject["clientPort"].get<uint16>();
					//							}
					//							user->Send(packetAchievement);
					//						}
					//					});*/

					//			}
					//		}
					//		else
					//			Logger::GetInstance().Warn(L"Error Selecting user's={0}, characterName={1}", user->GetAccountName(), packet.CharacterName);
					//		user->Send(sendPacket);
					//		//send terminate in case this fails. Either users doesnt have the character or the api returned an error.
					//	});
					//fwPacketListener::GetInstance().Connect(
					//	[](Lobby::UserSharedPtr& user, Lobby::Protocol::DeselectCharacter& packet)
					//	{
					//		Logger::GetInstance().Info("fwPacketListener :: userId@{0} :: protocol@DeselectCharacter", user->GetId());
					//		Net::Api api("DeselectCharacter");
					//		api << user->GetCharacterName();
					//		const Net::Answer result = api.RequestApi();
					//		Lobby::Protocol::DeselectCharacter sendPacket;
					//		if (result.errorCode == 0)
					//			sendPacket.Result = Lobby::Protocol::DeselectCharacter::Results::Ok;
					//		else {
					//			Logger::GetInstance().Warn(L"Could not deselect users={0}, character={1}.", user->GetId(), user->GetCharacterName());
					//			sendPacket.Result = Lobby::Protocol::DeselectCharacter::Results::NoResponse;
					//		}
					//		user->Send(sendPacket);
					//	});
					//fwPacketListener::GetInstance().Connect(
					//	[](Lobby::UserSharedPtr& user, Lobby::Protocol::ListSquareStatus& packet)
					//	{
					//		Logger::GetInstance().Info("fwPacketListener :: userId@{0} :: protocol@ListSquareStatus", user->GetId());
					//		if (user->IsCharacterSelected()) {
					//			Net::Api api("ListSquareStatus");
					//			auto result = api.RequestApi();
					//			Lobby::Protocol::ListSquareStatus sendPacket;
					//			if (result.errorCode == 0) {
					//				if (result.resultObject.size() > 0) {
					//					for (auto& x : result.resultObject) {
					//						XRated::SquareInfo info;
					//						info.StageGroupHash = x["stageGroupHash"].get<uint32>();
					//						info.AccessLevel = x["accessLevel"].get<uint16>();
					//						info.Capacity = x["capacity"].get<uint16>();
					//						{
					//							float helper = (float)x["connectionCount"].get<uint32>() / info.Capacity;
					//							if (helper > 0.95f)
					//								info.Status = XRated::SquareInfo::SquareStatus::Full;
					//							else if (helper > 0.70f)
					//								info.Status = XRated::SquareInfo::SquareStatus::Cloudy;
					//							else if (helper > 0.40f)
					//								info.Status = XRated::SquareInfo::SquareStatus::Normal;
					//							else
					//								info.Status = XRated::SquareInfo::SquareStatus::Quiet;
					//						}
					//						info.Name = StringUtil::ToUnicode(x["squareName"].get<std::string>());
					//						info.OrderNumber = x["orderNumber"].get<uint16>();
					//						sendPacket.SuqareList.push_back(info);
					//					}
					//				}
					//				else
					//					Logger::GetInstance().Warn("There are no square stages to be listed");
					//			}
					//			sendPacket.Result = static_cast<Lobby::Protocol::ListSquareStatus::Results>(result.errorCode);
					//			user->Send(sendPacket);
					//		}
					//		else
					//			user->Error(Lobby::ErrorLevel::Curious, L"");

					//	});
					//fwPacketListener::GetInstance().Connect(
					//	[](Lobby::UserSharedPtr& user, Lobby::Protocol::JoinSquare& packet)
					//	{
					//		Logger::GetInstance().Info("fwPacketListener :: userId@{0} :: protocol@JoinSquare", user->GetId());
					//		if (user->IsAccountAuthorized() && user->IsCharacterSelected()) {
					//			Lobby::Protocol::Join sendPacket;
					//			sendPacket.Result = Lobby::Protocol::Join::Results::NoResponse;
					//			Net::Api api("JoinSquare");
					//			json j;
					//			j["characterName"] = StringUtil::ToASCII(user->GetCharacterName());
					//			j["squareName"] = StringUtil::ToASCII(packet.SquareName);
					//			/*api.PostAsync(
					//				[&](Net::Answer& answer) {
					//					sendPacket.Result = static_cast<Lobby::Protocol::Join::Results>(answer.errorCode);
					//					if (answer.errorCode == 0 && !answer.resultObject.is_null()) {
					//						sendPacket.KeyCode = std::to_string(answer.resultObject["reservationSerial"].get<int>());
					//						sendPacket.Port = answer.resultObject["serverPort"].get<uint16>();
					//						sendPacket.ServerIp = answer.resultObject["serverAddress"].get<std::string>();
					//					}
					//					else
					//						Logger::GetInstance().Info("User[{0}] JoinSquare call failed.", user->GetId());
					//				}, j);*/

					//			user->Send(sendPacket);
					//		}
					//		else
					//			Logger::GetInstance().Info("User[{0}] is trying to call JoinSquare unauthenticated.", user->GetId());

					//	});
				});
		}
	}
}
