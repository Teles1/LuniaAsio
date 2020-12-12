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
								if (result.resultObjet["secondPassword"].get<std::string>() != "0" ) {
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
									if (result.resultObjet["characters"].size() > 0) {
										//populate characters package.
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
						if (result.errorCode == 0)
							user->PassedSecondPassword(true);
						Lobby::Protocol::SecondPasswordChecked sendPacket;
						sendPacket.PasswordInUse = (uint8)result.errorCode;
						sendPacket.FailCount = result.resultObjet["failCount"].get<uint32>();
						sendPacket.LockExpired = L"1990-01-01 01:00:00.000";//result.resultObjet["lockExpired"].get<std::wstring>();
						sendPacket.IsLocked = 0;
						sendPacket.Result = static_cast<Lobby::Protocol::SecondPasswordChecked::Results>(result.errorCode);
						user->Send(sendPacket);
					}
				});
			fwPacketListener::GetInstance().Connect(
				[](Lobby::UserSharedPtr& user, Lobby::Protocol::CreateSecondPassword& packet)
				{
					Logger::GetInstance().Info("fwPacketListener :: userId@{0} :: protocol@CreateSecondPassword", user->GetId());
					
				});
			fwPacketListener::GetInstance().Connect(
				[](Lobby::UserSharedPtr& user, Lobby::Protocol::DeleteSecondPassword& packet)
				{
					Logger::GetInstance().Info("fwPacketListener :: userId@{0} :: protocol@DeleteSecondPassword", user->GetId());
					
				});
			fwPacketListener::GetInstance().Connect(
				[](Lobby::UserSharedPtr& user, Lobby::Protocol::ModifySecondPassword& packet)
				{
					Logger::GetInstance().Info("fwPacketListener :: userId@{0} :: protocol@ModifySecondPassword", user->GetId());
					
				});
			fwPacketListener::GetInstance().Connect(
				[](Lobby::UserSharedPtr& user, Lobby::Protocol::SignInSecondPassword& packet)
				{
					Logger::GetInstance().Info("fwPacketListener :: userId@{0} :: protocol@SignInSecondPassword", user->GetId());
					
				});
			fwPacketListener::GetInstance().Connect(
				[](Lobby::UserSharedPtr& user, Lobby::Protocol::Terminate& packet)
				{
					Logger::GetInstance().Info("fwPacketListener :: userId@{0} :: protocol@Terminate", user->GetId());
					//Gotta safely disconect the user. 
					Lobby::Protocol::Terminate sendPacket;
					sendPacket.Result = Lobby::Protocol::Terminate::Results::Ok;
					user->Send(sendPacket);
					//Net::UserRegistry::GetInstance().RemoveUser(user);
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
					//sendPacket.Result = static_cast<Lobby::Protocol::CreateCharacter::Results>(result.errorCode);
					if (result.errorCode == 0) {
						sendPacket.CharacterInfo.CharacterName = result.resultObjet["characterName"].get<std::wstring>();
						sendPacket.CharacterInfo.CharacterSerial = result.resultObjet["Id"].get<int64>();
						sendPacket.CharacterInfo.VirtualIdCode = result.resultObjet["Id"].get<uint32>();
						sendPacket.CharacterInfo.ClassType = static_cast<XRated::Constants::ClassType>(result.resultObjet["ClassNumber"].get<int>());
						sendPacket.CharacterInfo.Level = result.resultObjet["StageLevel"].get<int>();
						sendPacket.CharacterInfo.Exp = result.resultObjet["StageExp"].get<int>();
						sendPacket.CharacterInfo.PvpLevel = result.resultObjet["PvpLevel"].get<int>();
						sendPacket.CharacterInfo.PvpExp = result.resultObjet["PvpExp"].get<int>();
						sendPacket.CharacterInfo.WarLevel = result.resultObjet["WarLevel"].get<int>();
						sendPacket.CharacterInfo.WarExp = result.resultObjet["WarExp"].get<int>();
						sendPacket.CharacterInfo.StateFlags = static_cast<XRated::CharacterStateFlags>(result.resultObjet["InstantStateFlag"].get<int>());
						sendPacket.CharacterInfo.RebirthCount = result.resultObjet["characterRebirth"]["rebirthCount"].get<uint32>();
						sendPacket.CharacterInfo.StoredLevel = result.resultObjet["characterRebirth"]["storedLevel"].get<uint16>();

						for (auto y : result.resultObjet["characterLicenses"].get<json>()) {
							sendPacket.CharacterInfo.Licenses.push_back(XRated::StageLicense());
							sendPacket.CharacterInfo.Licenses.back().StageGroupHash = y["stageHash"].get<uint32>();
							sendPacket.CharacterInfo.Licenses.back().Level = y["stageHash"].get<uint16>();
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
		});
}
