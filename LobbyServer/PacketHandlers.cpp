#include "fwPacketListener.h"
#include "../Core/Utils/InitFunction.h"
#include <Core/Utils/Math/Random.h>
#include "./Network/Api/Api.h"
#include "UserRegistry.h"
#include <Core/Utils/ThreadPool.h>
#include <Core/Utils/ConfigReader.h>

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
					user->SatisfyAlivePingWait();
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
									user.get()->m_NumberOfSlots = result.resultObject["characterSlots"].get<uint8>();
									user->m_AccountLicenses  = result.resultObject["accountsLicense"].get<int32>();
								}
								
								//Lock user's account if second password is set 
								if (result.resultObject["secondPassword"].get<bool>()) {
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
									if(!result.resultObject["characters"].is_null())
										for (auto y : result.resultObject["characters"]) {
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
						sendPacket.FailCount = result.resultObject["failCount"].get<uint32>();
						sendPacket.LockExpired = StringUtil::ToUnicode(result.resultObject["lockExpired"].get<std::string>());
						sendPacket.IsLocked = result.resultObject["isLocked"].get<uint8>();
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
						sendPacket.CharacterInfo.CharacterName = StringUtil::ToUnicode(result.resultObject["characterName"].get<std::string>());
						sendPacket.CharacterInfo.CharacterSerial = result.resultObject["id"].get<int64>();
						sendPacket.CharacterInfo.VirtualIdCode = result.resultObject["id"].get<uint32>();
						sendPacket.CharacterInfo.ClassType = static_cast<XRated::Constants::ClassType>(result.resultObject["classNumber"].get<int>());
						sendPacket.CharacterInfo.Level = result.resultObject["stageLevel"].get<uint16>();
						sendPacket.CharacterInfo.Exp = result.resultObject["stageExp"].get<uint32>();
						sendPacket.CharacterInfo.PvpLevel = result.resultObject["pvpLevel"].get<uint16>();
						sendPacket.CharacterInfo.PvpExp = result.resultObject["pvpExp"].get<uint32>();
						sendPacket.CharacterInfo.WarLevel = result.resultObject["warLevel"].get<uint16>();
						sendPacket.CharacterInfo.WarExp = result.resultObject["warExp"].get<uint32>();
						sendPacket.CharacterInfo.StateFlags = static_cast<XRated::CharacterStateFlags>(result.resultObject["instantStateFlag"].get<int>());
						sendPacket.CharacterInfo.RebirthCount = result.resultObject["characterRebirth"]["rebirthCount"].get<uint16>();
						sendPacket.CharacterInfo.StoredLevel = result.resultObject["characterRebirth"]["storedLevel"].get<uint16>();
						
						for (auto y : result.resultObject["characterLicenses"].get<json>()) { //[{"stageHash":19999,"accessLevel":1,"difficulty": 1}]
							sendPacket.CharacterInfo.Licenses.push_back(XRated::StageLicense(y["stageHash"].get<uint32>(), y["accessLevel"].get<uint16>(), y["difficulty"].get<uint8>()));
						}

						for (auto y : result.resultObject["items"].get<json>()) {
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

					Net::Api api("SaveKeySetting");
					api << packet.Account;
					const Net::Answer result = api.RequestPost(StringUtil::ToASCII(packet.Keycodes));
					if (result.errorCode != 0) {
						//key save error;
						return;
					}
				});
			fwPacketListener::GetInstance().Connect(
				[](Lobby::UserSharedPtr& user, Lobby::Protocol::Alive& packet)
				{
					Logger::GetInstance().Info("fwPacketListener :: userId@{0} :: protocol@Alive", user->GetId());
					Lobby::Alive::AliveData answer(packet.Index,packet.Value1,packet.Value2,packet.Value3);

					user->SetAliveAsLastTickAlivePing(answer);
				});
			fwPacketListener::GetInstance().Connect(
				[](Lobby::UserSharedPtr& user, Lobby::Protocol::SelectCharacter& packet)
				{
					Logger::GetInstance().Info("fwPacketListener :: userId@{0} :: protocol@SelectCharacter", user->GetId());
					Net::Api api("SelectCharacter");
					//api << Config::GetInstance().m_ServerName;
					api << user->GetAccountName();
					api << packet.CharacterName;
					auto result = api.RequestApi();
					if (result.errorCode == 0 && user->IsAValidCharacterName(packet.CharacterName) && user->SetSelectedCharacter(packet.CharacterName)) {
						Lobby::Protocol::SelectCharacter sendPacket;
						sendPacket.Result = static_cast<Lobby::Protocol::SelectCharacter::Results>(0);
						sendPacket.CharacterName = StringUtil::ToUnicode(result.resultObject["characterName"].get<std::string>());

						// TODO : Fix it latter with achievement
						sendPacket.CharacterStates = static_cast<XRated::CharacterStateFlags>(0);

						user->Send(sendPacket);
					}
					//send terminate in case this fails. Either users doesnt have the character or the api returned an error.
				});
			fwPacketListener::GetInstance().Connect(
				[](Lobby::UserSharedPtr& user, Lobby::Protocol::DeselectCharacter& packet)
				{
					Logger::GetInstance().Info("fwPacketListener :: userId@{0} :: protocol@DeselectCharacter", user->GetId());
					Net::Api api("DeselectCharacter");
					api << user->GetCharacterName();
					const Net::Answer result = api.RequestApi();
					Lobby::Protocol::DeselectCharacter sendPacket;
					if (result.errorCode == 0)
						sendPacket.Result = Lobby::Protocol::DeselectCharacter::Results::Ok;
					else {
						Logger::GetInstance().Warn(L"Could not deselect users={0}, character={1}.", user->GetId(), user->GetCharacterName());
						sendPacket.Result = Lobby::Protocol::DeselectCharacter::Results::NoResponse;
					}
					user->Send(sendPacket);
				});
			fwPacketListener::GetInstance().Connect(
				[](Lobby::UserSharedPtr& user, Lobby::Protocol::ListSquareStatus& packet)
				{
					Logger::GetInstance().Info("fwPacketListener :: userId@{0} :: protocol@ListSquareStatus", user->GetId());
					if (user->IsCharacterSelected()) {
						Net::Api api("ListSquareStatus");
						auto result = api.RequestApi();
						Lobby::Protocol::ListSquareStatus sendPacket;
						if (result.errorCode == 0) {
							if (result.resultObject.size() > 0) {
								for (auto& x : result.resultObject) {
									XRated::SquareInfo info;
									info.StageGroupHash = x["stageGroupHash"].get<uint32>();
									info.AccessLevel = x["accessLevel"].get<uint16>();
									info.Capacity = x["capacity"].get<uint16>();
									{
										float helper = (float)x["connectionCount"].get<uint32>() / info.Capacity;
										if (helper > 0.95f)
											info.Status = XRated::SquareInfo::SquareStatus::Full;
										else if (helper > 0.70f)
											info.Status = XRated::SquareInfo::SquareStatus::Cloudy;
										else if (helper > 0.40f)
											info.Status = XRated::SquareInfo::SquareStatus::Normal;
										else
											info.Status = XRated::SquareInfo::SquareStatus::Quiet;
									}
									info.Name = StringUtil::ToUnicode(x["squareName"].get<std::string>());
									info.OrderNumber = x["orderNumber"].get<uint16>();
									sendPacket.SuqareList.push_back(info);
								}
							}
							else
								Logger::GetInstance().Warn("There are no square stages to be listed");
						}
						sendPacket.Result = static_cast<Lobby::Protocol::ListSquareStatus::Results>(result.errorCode);
						user->Send(sendPacket);
					}
					else
						user->Error(Lobby::ErrorLevel::Curious, L"");
					
				});
			fwPacketListener::GetInstance().Connect(
				[](Lobby::UserSharedPtr& user, Lobby::Protocol::JoinSquare& packet)
				{
					Logger::GetInstance().Info("fwPacketListener :: userId@{0} :: protocol@JoinSquare", user->GetId());
					if (user->IsAccountAuthorized() && user->IsCharacterSelected()) {
						Lobby::Protocol::Join sendPacket;
						sendPacket.Result = Lobby::Protocol::Join::Results::NoResponse;
						Net::Api api("JoinSquare");
						api << user->GetCharacterName() << packet.SquareName;
						api.GetAsync(
							[&](Net::Answer& answer) {
								sendPacket.Result = static_cast<Lobby::Protocol::Join::Results>(answer.errorCode);
								if (answer.errorCode == 0 && !answer.resultObject.is_null()) {
									sendPacket.KeyCode = answer.resultObject["reservationSerial"].get<std::string>();
									sendPacket.Port = answer.resultObject["serverPort"].get<uint16>();
									sendPacket.ServerIp = answer.resultObject["serverAddress"].get<std::string>();
								}
								else
									Logger::GetInstance().Info("User[{0}] JoinSquare call failed.", user->GetId());
							});

						user->Send(sendPacket);
					}
					else
						Logger::GetInstance().Info("User[{0}] is trying to call JoinSquare unauthenticated.", user->GetId());
					
				});
		});
}
