#include "fwPacketListener.h"
#include "../Core/Utils/InitFunction.h"
#include <Core/Utils/Math/Random.h>
#include "./Network/Api/Api.h"
#include "UserRegistry.h"
#include <Core/Utils/ThreadPool.h>

namespace Lunia {
	static utils::InitFunction init([]()
		{
			fwPacketListener::GetInstance().Connect(
				[](Lobby::UserSharedPtr& user, Lobby::Protocol::Head& packet)
				{
					Logger::GetInstance().Info("fwPacketListener :: userId@{0} :: protocol@Head", user->GetId());
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
			fwPacketListener::GetInstance().Connect(
				[](Lobby::UserSharedPtr& user, Lobby::Protocol::Auth& packet)
				{
					Logger::GetInstance().Info("fwPacketListener :: userId@{0} :: protocol@Auth", user->GetId());
					user->SetLocale(packet.Locale);
					Net::Api api("CheckAccount");
					api << packet.AccountId;
					api << packet.EncryptedPassword;
					api << user->GetPeerAddress();
					const Net::Answer result = api.RequestApi();
					Lobby::Protocol::Auth sendPacket;

					sendPacket.Result = static_cast<Lobby::Protocol::Auth::Results>(result.errorCode);

					switch (sendPacket.Result)
					{
					case Lobby::Protocol::Auth::Results::Ok:
					{
						user->SetAccountName(packet.AccountId); //It's fine because we already know that the account exists and the user is authenticated

						sendPacket.AccountId = std::move(packet.AccountId);

						Net::UserRegistry::GetInstance().AuthenticateUser(user);
						//send Character List
						auto listCharacter = [&](int threadId, Lobby::UserSharedPtr& user) {
							Net::Api api("ListCharacters");
							api << user->GetAccountName();
							auto result(api.RequestApi());
							if (result.errorCode == 0) {
								
								//CharacterLicenses
								{
									user.get()->m_NumberOfSlots = result.resultObjet["characterSlots"].get<uint8>();
									user->m_AccountLicenses  = result.resultObjet["accountsLicense"].get<int32>();
								}
								
								//Lock user's account if second password is set 
								if (result.resultObjet["secondPassword"].get<std::string>() == "0" ) {
									user->PassedSecondPassword(true);
								}

								{
									Lobby::Protocol::CharacterSlots slots;
									slots.NumberOfSlots = user->m_NumberOfSlots;
									for (int i = 0; i < 16; i++) {
										if (static_cast<bool>(user->m_AccountLicenses & (1 << i)))
											slots.CharacterLicenses.push_back(i);
									}
									user->Send(slots);
								}
								
								{
									if(result.resultObjet["characters"].is_object())
										for (auto y : result.resultObjet["characters"].get<json>()) {
											user->m_Characters.push_back(XRated::LobbyPlayerInfo());
											XRated::LobbyPlayerInfo& info = user->m_Characters.back();
											info.CharacterName = StringUtil::ToUnicode(y["characterName"].get<std::string>());
											info.CharacterSerial = y["id"].get<int64>();
											info.VirtualIdCode = y["id"].get<uint32>();
											info.ClassType = static_cast<XRated::Constants::ClassType>(y["classNumber"].get<int>());
											info.Level = y["stageLevel"].get<uint16>();
											info.Exp = y["stageExp"].get<uint32>();
											info.PvpLevel = y["pvpLevel"].get<uint16>();
											info.PvpExp = y["pvpExp"].get<uint32>();
											info.WarLevel = y["warLevel"].get<uint16>();
											info.WarExp = y["warExp"].get<uint32>();
											info.StateFlags = static_cast<XRated::CharacterStateFlags>(y["instantStateFlag"].get<int>());
											info.RebirthCount = y["characterRebirth"]["rebirthCount"].get<uint16>();
											info.StoredLevel = y["characterRebirth"]["storedLevel"].get<uint16>();

											for (auto y : y["characterLicenses"].get<json>()) { //[{"stageHash":19999,"accessLevel":1,"difficulty": 1}]
												info.Licenses.push_back(XRated::StageLicense(y["stageHash"].get<uint32>(), y["accessLevel"].get<uint16>(), y["difficulty"].get<uint8>()));
											}

											for (auto y : y["items"].get<json>()) {
												XRated::ItemSlot slot;
												slot.Position.Bag = y["bagNumber"].get<uint8>(); // equipment slots at
												slot.Position.Position = y["positionNumber"].get<uint8>();
												slot.Stacked = 1; // equipments cannot be stacked
												slot.Id = y["itemHash"].get<uint32>();
												slot.instanceEx.Instance = y["instance"].get<int64>();
												slot.instanceEx.ExpireDate.Parse(StringUtil::ToUnicode(y["itemExpire"].get<std::string>()));
												info.Equipments.push_back(slot);
											}
										}
									Lobby::Protocol::ListCharacter sendPacket;
									sendPacket.Characters = user->m_Characters;
									user->Send(sendPacket);
								}
							}
							else
								Logger::GetInstance().Warn("[{}] Error requesting for the user's character list", user->GetId());
						};
						Utils::thread_pool::GetInstance().push(listCharacter, user);
						break;
					}
					default:
						//Net::UserRegistry::GetInstance().RemoveUser(user);
						break;
					}

					user->Send(sendPacket);
				});
			fwPacketListener::GetInstance().Connect(
				[](Lobby::UserSharedPtr& user, Lobby::Protocol::CheckSecondPassword& packet)
				{
					Logger::GetInstance().Info("fwPacketListener :: userId@{0} :: protocol@CheckSecondPassword", user->GetId());
					Net::Api api("Auth_2nd_Check");
					api << user->GetAccountName();
					const Net::Answer result = api.RequestApi();
					if (result.errorCode == -1) {
						//Api call failed. Do something
						
					}
					else {
						Lobby::Protocol::SecondPasswordChecked sendPacket;
						if (result.errorCode == 0)
							user->PassedSecondPassword(true);

						sendPacket.PasswordInUse = (uint8)result.errorCode;
						sendPacket.FailCount = result.resultObjet["failCount"].get<uint32>();
						sendPacket.LockExpired = StringUtil::ToUnicode(result.resultObjet["lockExpired"].get<std::string>());
						sendPacket.IsLocked = result.resultObjet["isLocked"].get<uint8>();
						sendPacket.Result = static_cast<Lobby::Protocol::SecondPasswordChecked::Results>(0);
						user->Send(sendPacket);
					}
				});
			fwPacketListener::GetInstance().Connect(
				[](Lobby::UserSharedPtr& user, Lobby::Protocol::CreateSecondPassword& packet)
				{
					Logger::GetInstance().Info("fwPacketListener :: userId@{0} :: protocol@CreateSecondPassword", user->GetId());
					Net::Api api("Auth_2nd_Create");
					api << user->GetAccountName() << packet.Password;
					const Net::Answer result = api.RequestApi();
					if (result.errorCode == 0) {
						Lobby::Protocol::SecondPasswordCreated sendPacket;
						sendPacket.Result = static_cast<Lobby::Protocol::SecondPasswordCreated::Results>(result.errorCode);
						user->Send(sendPacket);
					}
				});
			fwPacketListener::GetInstance().Connect(
				[](Lobby::UserSharedPtr& user, Lobby::Protocol::DeleteSecondPassword& packet)
				{
					Logger::GetInstance().Info("fwPacketListener :: userId@{0} :: protocol@DeleteSecondPassword", user->GetId());
					Net::Api api("Auth_2nd_Delete");
					api << user->GetAccountName() << packet.Password;
					const Net::Answer result = api.RequestApi();
					if (result.errorCode == 0) {
						Lobby::Protocol::SecondPasswordDeleted sendPacket;
						sendPacket.Result = static_cast<Lobby::Protocol::SecondPasswordDeleted::Results>(result.errorCode);
						user->Send(sendPacket);
					}
					
				});
			fwPacketListener::GetInstance().Connect(
				[](Lobby::UserSharedPtr& user, Lobby::Protocol::ModifySecondPassword& packet)
				{
					Logger::GetInstance().Info("fwPacketListener :: userId@{0} :: protocol@ModifySecondPassword", user->GetId());
					Net::Api api("Auth_2nd_Modify");
					api << user->GetAccountName() << packet.Password << packet.NewPassword;
					const Net::Answer result = api.RequestApi();
					if (result.errorCode == 0) {
						Lobby::Protocol::SecondPasswordModified sendPacket;
						sendPacket.Result = static_cast<Lobby::Protocol::SecondPasswordModified::Results>(result.errorCode);
						user->Send(sendPacket);
					}
					
				});
			fwPacketListener::GetInstance().Connect(
				[](Lobby::UserSharedPtr& user, Lobby::Protocol::SignInSecondPassword& packet)
				{
					Logger::GetInstance().Info("fwPacketListener :: userId@{0} :: protocol@SignInSecondPassword", user->GetId());
					Net::Api api("Auth_2nd_Sign");
					api << user->GetAccountName() << packet.Password;
					const Net::Answer result = api.RequestApi();

					Lobby::Protocol::SecondPasswordSignedIn sendPacket;

					sendPacket.Result = static_cast<Lobby::Protocol::SecondPasswordSignedIn::Results>(result.errorCode);

					user->Send(sendPacket);

				});
			fwPacketListener::GetInstance().Connect(
				[](Lobby::UserSharedPtr& user, Lobby::Protocol::Terminate& packet)
				{
					Logger::GetInstance().Info("fwPacketListener :: userId@{0} :: protocol@Terminate", user->GetId());
					//Gotta safely disconect the user. 
					Net::Api api("Terminate");
					api << user->GetAccountName();
					const Net::Answer result = api.RequestApi();
					Lobby::Protocol::Terminate sendPacket;
					sendPacket.Result = Lobby::Protocol::Terminate::Results::Ok;
					user->Send(sendPacket);
					Net::UserRegistry::GetInstance().RemoveUser(user);
				});
			fwPacketListener::GetInstance().Connect(
				[](Lobby::UserSharedPtr& user, Lobby::Protocol::CreateCharacter& packet)
				{
					Logger::GetInstance().Info("fwPacketListener :: userId@{0} :: protocol@CreateCharacter", user->GetId());
					
					bool checkLicense = true;
					if (XRated::Constants::IsDefaultClass(packet.ClassType) == false)
						checkLicense = user->DoesHaveLicense(packet.ClassType);

					Net::Api api("CreateCharacter");
					api << user->GetAccountName() << packet.Name << static_cast<int>(packet.ClassType) << checkLicense;
					const Net::Answer result = api.RequestApi();
					Lobby::Protocol::CreateCharacter sendPacket;
					sendPacket.Result = static_cast<Lobby::Protocol::CreateCharacter::Results>(result.errorCode);
					if (result.errorCode == 0) {
						sendPacket.CharacterInfo.CharacterName = StringUtil::ToUnicode(result.resultObjet["characterName"].get<std::string>());
						sendPacket.CharacterInfo.CharacterSerial = result.resultObjet["id"].get<int64>();
						sendPacket.CharacterInfo.VirtualIdCode = result.resultObjet["id"].get<uint32>();
						sendPacket.CharacterInfo.ClassType = static_cast<XRated::Constants::ClassType>(result.resultObjet["classNumber"].get<int>());
						sendPacket.CharacterInfo.Level = result.resultObjet["stageLevel"].get<uint16>();
						sendPacket.CharacterInfo.Exp = result.resultObjet["stageExp"].get<uint32>();
						sendPacket.CharacterInfo.PvpLevel = result.resultObjet["pvpLevel"].get<uint16>();
						sendPacket.CharacterInfo.PvpExp = result.resultObjet["pvpExp"].get<uint32>();
						sendPacket.CharacterInfo.WarLevel = result.resultObjet["warLevel"].get<uint16>();
						sendPacket.CharacterInfo.WarExp = result.resultObjet["warExp"].get<uint32>();
						sendPacket.CharacterInfo.StateFlags = static_cast<XRated::CharacterStateFlags>(result.resultObjet["instantStateFlag"].get<int>());
						sendPacket.CharacterInfo.RebirthCount = result.resultObjet["characterRebirth"]["rebirthCount"].get<uint16>();
						sendPacket.CharacterInfo.StoredLevel = result.resultObjet["characterRebirth"]["storedLevel"].get<uint16>();

						for (auto y : result.resultObjet["characterLicenses"].get<json>()) { //[{"stageHash":19999,"accessLevel":1,"difficulty": 1}]
							sendPacket.CharacterInfo.Licenses.push_back(XRated::StageLicense(y["stageHash"].get<uint32>(), y["accessLevel"].get<uint16>(), y["difficulty"].get<uint8>()));
						}

						for (auto y : result.resultObjet["items"].get<json>()) {
							XRated::ItemSlot slot;
							slot.Position.Bag = y["bagNumber"].get<uint8>(); // equipment slots at
							slot.Position.Position = y["positionNumber"].get<uint8>();
							slot.Stacked = 1; // equipments cannot be stacked
							slot.Id = y["itemHash"].get<uint32>();
							slot.instanceEx.Instance = y["instance"].get<int64>();
							slot.instanceEx.ExpireDate.Parse(y["itemExpire"].get<std::wstring>());
							sendPacket.CharacterInfo.Equipments.push_back(slot);
						}
						user->m_Characters.push_back(sendPacket.CharacterInfo);
					}
					user->Send(sendPacket);
				});

			fwPacketListener::GetInstance().Connect(
				[](Lobby::UserSharedPtr& user, Lobby::Protocol::DeleteCharacter& packet)
				{
					Logger::GetInstance().Info("fwPacketListener :: userId@{0} :: protocol@DeleteCharacter", user->GetId());

					if (!user->IsAccountAuthorized()) {
						Logger::GetInstance().Error(L"Non authorized user trying to delete a AccountName: {0}, characterName: {1}", user->GetAccountName(), packet.Name);
						Net::UserRegistry::GetInstance().RemoveUser(user);
						return;
					}
					if (!user->IsAValidCharacterName(packet.Name)) {
						Logger::GetInstance().Error(L"User is trying to delete a characterName that does not exist. AccountName: {0}, characterName: {1}", user->GetAccountName(), packet.Name);
						Net::UserRegistry::GetInstance().RemoveUser(user);
						return;
					}
					Net::Api api("DeleteCharacter");
					api << packet.Name;
					const Net::Answer result = api.RequestApi();
					Lobby::Protocol::DeleteCharacter sendPacket;
					sendPacket.Result = static_cast<Lobby::Protocol::DeleteCharacter::Results>(result.errorCode);
					if (result.errorCode == 0) {
						sendPacket.DeletedCharacter = packet.Name;
						if (!user->DeleteCharacter(packet.Name)) {
							Logger::GetInstance().Error(L"Could not Delete the specified characterName AccountName: {0}, characterName: {1}", user->GetAccountName(), packet.Name);
							Net::UserRegistry::GetInstance().RemoveUser(user);
						}
					}
					user->Send(sendPacket);
				});

			fwPacketListener::GetInstance().Connect(
				[](Lobby::UserSharedPtr& user, Lobby::Protocol::SaveKeySetting& packet)
				{
					Logger::GetInstance().Info("fwPacketListener :: userId@{0} :: protocol@SaveKeySetting", user->GetId());

					json j = StringUtil::ToASCII(packet.Keycodes);
					cpr::Response r = cpr::Post(cpr::Url("http://localhost:51542/Lobby/SaveKeySetting/Teste"), cpr::Body{ j.dump() }, cpr::Header{ {"Content-Type", "application/json"} },  cpr::Timeout{ 1000 });

				});
		});
}
