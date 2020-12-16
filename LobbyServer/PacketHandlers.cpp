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
						user->SetAccountName(packet.AccountId);

						// result.resultObjet["secondPassword"].get<bool>();
						bool hasSecondPasswordAuthentication = true; 
						if (hasSecondPasswordAuthentication)
						{
							user->SetHasSecondPasswordAuthentication();

							{
								Lobby::Protocol::CharacterSlots toSendPacket;

								toSendPacket.NumberOfSlots = 988989897;
								toSendPacket.CharacterLicenses = { };

								user->Send(toSendPacket);
							}
							
							{
								Lobby::Protocol::ListCharacter toSendPacket;

								toSendPacket.Characters = { };

								user->Send(toSendPacket);
							}
						
						}
						else
						{
							Net::UserRegistry::GetInstance().AuthenticateUser(user);
						}

						sendPacket.AccountId = std::move(packet.AccountId);
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
					
					Lobby::Protocol::SecondPasswordChecked toSendPacket;

					toSendPacket.Result = Lobby::Protocol::SecondPasswordChecked::Results::Ok;
					toSendPacket.PasswordInUse = 0;
					toSendPacket.FailCount = 0;
					toSendPacket.LockExpired = L"";
					toSendPacket.IsLocked = 0;

					if (user->HasSecondPasswordAuthentication())
					{
						Net::Api api("Auth_2nd_Check");

						api << user->GetAccountName();

						const Net::Answer result = api.RequestApi();

						auto resultCode = static_cast<Lobby::Protocol::SecondPasswordChecked::Results>(result.errorCode);

						switch (resultCode)
						{
						case Lobby::Protocol::SecondPasswordChecked::Results::Ok:

							Net::UserRegistry::GetInstance().AuthenticateUser(user);

							toSendPacket.PasswordInUse = (uint8)result.errorCode;
							
							toSendPacket.FailCount = result.resultObjet["failCount"].get<uint32>();
							
							toSendPacket.LockExpired = StringUtil::ToUnicode(result.resultObjet["lockExpired"].get<std::string>());
							
							toSendPacket.IsLocked = result.resultObjet["isLocked"].get<uint8>();
							
							toSendPacket.Result = static_cast<Lobby::Protocol::SecondPasswordChecked::Results>(0);

							break;
						default:
							// NoResponse
							break;
						}
					}

					user->Send(toSendPacket);
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

					if (!user->IsAuthenticated()) {
						Logger::GetInstance().Error(L"Non authorized user trying to delete a AccountName: {0}, characterName: {1}", user->GetAccountName(), packet.Name);
						Net::UserRegistry::GetInstance().RemoveUser(user);
						return;
					}
					if (!user->HasAnyCharacterWithName(packet.Name)) {
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
					
					if (!user->HasAnyCharacterWithName(packet.CharacterName))
						return;
					// not exist character name(%s) is requested to select. this might be hacked.

					if (user->IsAuthenticated() /*&& !user->HasActiveCharacter()*/)
					{
						Net::Api request("SelectCharacter");

						//request << sender->GetAccountName()
						//		<< packet.CharacterName;

						Lobby::UserWeakPtr userWeak{ user };

						request.GetAsync([userWeak](Lunia::Net::Answer& answ)
							{
								auto newUserLocked = userWeak.lock();

								if (!newUserLocked)
									return;

								if (!newUserLocked->IsAuthenticated())
								{
									// Client dies
									return;
								}

								Lobby::Protocol::SelectCharacter toSendPacket;

								auto selectCharacterResult = static_cast<Lobby::Protocol::SelectCharacter::Results>(answ.errorCode);

								toSendPacket.Result = selectCharacterResult;

								/*
								if (selectCharacterResult == Lobby::Protocol::SelectCharacter::Results::Ok)
								{
									
									auto newActiveCharacter = answ.resultObjet["A"].get<int64>();

									newUserLocked->SetActiveCharacter(newActiveCharacter);
									newUserLocked->SetActiveCharacterStateFlag();

									toSendPacket.SelectedCharacter = newActiveCharacter;
									toSendPacket.CharacterStates = answ.resultObjet["A"].get<int64>();
								}
								*/
							});
					}
				});
			fwPacketListener::GetInstance().Connect(
				[](Lobby::UserSharedPtr& user, Lobby::Protocol::ListSquareStatus& packet)
				{
					Logger::GetInstance().Info("fwPacketListener :: userId@{0} :: protocol@ListSquareStatus", user->GetId());
					Net::Api api("ListSquareStatus");
					api << Config::GetInstance().m_ServerName;
					auto result = api.RequestApi();
					Lobby::Protocol::ListSquareStatus sendPacket;
					if (result.errorCode == 0) {
						if (result.resultObjet.size() > 0) {
							for (auto& x : result.resultObjet) {
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
					user->Send(sendPacket);
				});
		});
}
