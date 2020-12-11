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
									auto j_array = result.resultObjet["accountsLicense"].get<json>();
									for (json::iterator it = j_array.begin(); it != j_array.end(); it++)
										if ((*it).get<uint16>() > 16 || (*it).get<uint16>() < 0)
											Logger::GetInstance().Exception("[{0}] AccountLicenses out of boundries.", user->GetId());
										else
											user->m_AccountLicenses.push_back((*it).get<uint16>());
								}
								
								//Lock user's account if second password is set 
								if (result.resultObjet["secondPassword"].get<std::string>() != "0" ) {
									user->PassedSecondPassword(true);
								}

								{
									Lobby::Protocol::CharacterSlots slots;
									slots.NumberOfSlots = user->m_NumberOfSlots;
									slots.CharacterLicenses = user->m_AccountLicenses;
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
					Net::UserRegistry::GetInstance().RemoveUser(user);
				});
		});
}
