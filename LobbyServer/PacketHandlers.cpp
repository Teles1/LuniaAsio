#include "fwPacketListener.h"
#include "../Core/Utils/InitFunction.h"
#include <Core/Utils/Math/Random.h>
#include "./Network/Api/Api.h"
#include <LobbyServer/Common.h>
#include <Core/Utils/ThreadPool.h>
#include <Core/Utils/ConfigReader.h>

#include "Main.h"

namespace Lunia {
	static utils::InitFunction init([]()
		{
			// typedef std::shared_ptr<ClientProxyLobby> ClientProxyLobbySharedPtr;

			g_gameServer->Proxy.PacketHandler.Connect([](std::shared_ptr<ClientProxyLobby>& client, Lobby::Protocol::Head& packet)
			{
				Lobby::Protocol::Head sendPacket;

				if (packet.ProtocolVersion == Constants::Version)
					sendPacket.Result = Lobby::Protocol::Head::Results::Ok;
				else
					sendPacket.Result = Lobby::Protocol::Head::Results::VersionMismatched;

				sendPacket.EncryptKey = Math::Random();

				sendPacket.ServerTime = DateTime::Now();

				sendPacket.UserIP = client->GetPeerAddress();

				client->SetEncryptionKey(sendPacket.EncryptKey);

				client->MakeSocketAsyncWriteSerializable(sendPacket);

				client->Ping(); /* TODO Implement ClientProxyLobby ping */
			});

			g_gameServer->Proxy.PacketHandler.Connect([](std::shared_ptr<ClientProxyLobby>& client, Lobby::Protocol::Auth& packet)
			{
				client->SetLocale(packet.Locale);

				Net::Api api("CheckAccount");

				api << packet.AccountId;
				api << packet.EncryptedPassword;
				api << client->GetPeerAddress();

				const Net::Answer result = api.RequestApi();

				Lobby::Protocol::Auth sendPacket;

				sendPacket.Result = static_cast<Lobby::Protocol::Auth::Results>(result.errorCode);

				switch (sendPacket.Result)
				{
				case Lobby::Protocol::Auth::Results::Ok:
				{
					client->SetAccountName(packet.AccountId);

					sendPacket.AccountId = std::move(packet.AccountId);

					Net::Api api("ListCharacters");

					api << client->GetAccountName();

					auto result(api.RequestApi());

					if (result.errorCode == 0) {

						//CharacterLicenses
						{
							client->m_numOfCharacterSlots = result.resultObject["characterSlots"].get<uint8>();
							client->m_accountLicense = result.resultObject["accountsLicense"].get<int32>();
						}

						//Lock user's account if second password is set 
						if (result.resultObject["secondPassword"].get<bool>())
							client->SetHasSecondPasswordAuthentication();
						else
							g_gameServer->Proxy.ClientRegistry.AuthenticateClient(client);

						{
							Lobby::Protocol::CharacterSlots sendPacketCharacterSlots;

							sendPacketCharacterSlots.NumberOfSlots = client->m_numOfCharacterSlots;

							for (int i = 0; i < 16; i++) {
								if (static_cast<bool>(client->m_accountLicense & (1 << i)))
									sendPacketCharacterSlots.CharacterLicenses.push_back(i);
							};

							client->MakeSocketAsyncWriteSerializable(sendPacketCharacterSlots);
						}

						{
							if (!result.resultObject["characters"].is_null())
							{
								for (auto y : result.resultObject["characters"]) 
								{
									client->m_characters.push_back(XRated::LobbyPlayerInfo());

									XRated::LobbyPlayerInfo& info = client->m_characters.back();

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
							}

							Lobby::Protocol::ListCharacter sendPacketListCharacter;

							sendPacketListCharacter.Characters = client->m_characters;

							client->MakeSocketAsyncWriteSerializable(sendPacketListCharacter);
						}
					}
					else
					{
						/*
							Error code is not 0
						*/
						Logger::GetInstance().Warn("[{}] Error requesting for the user's character list", client->GetId());
					}

					break;
				}
				default:
					//g_gameServer->Proxy.ClientRegistry.DropClient(user);
					break;
				}

				client->MakeSocketAsyncWriteSerializable(sendPacket);
			});

			g_gameServer->Proxy.PacketHandler.Connect([](std::shared_ptr<ClientProxyLobby>& client, Lobby::Protocol::CheckSecondPassword& packet)
			{
				Logger::GetInstance().Info("fwPacketListener :: userId@{0} :: protocol@CheckSecondPassword", client->GetId());

				Net::Api api("Auth_2nd_Check");

				api << client->GetAccountName();

				const Net::Answer result = api.RequestApi();

				if (result.errorCode == -1) {
					//Api call failed. Do something
						
				}
				else 
				{
					Lobby::Protocol::SecondPasswordChecked sendPacket;

					if (result.errorCode == 0)
						g_gameServer->Proxy.ClientRegistry.AuthenticateClient(client);

					sendPacket.PasswordInUse = (uint8)result.errorCode;
					sendPacket.FailCount = result.resultObject["failCount"].get<uint32>();
					sendPacket.LockExpired = StringUtil::ToUnicode(result.resultObject["lockExpired"].get<std::string>());
					sendPacket.IsLocked = result.resultObject["isLocked"].get<uint8>();
					sendPacket.Result = static_cast<Lobby::Protocol::SecondPasswordChecked::Results>(0);

					client->MakeSocketAsyncWriteSerializable(sendPacket);
				}
			});

			g_gameServer->Proxy.PacketHandler.Connect([](std::shared_ptr<ClientProxyLobby>& client, Lobby::Protocol::CreateSecondPassword& packet)
			{
				Logger::GetInstance().Info("fwPacketListener :: userId@{0} :: protocol@CreateSecondPassword", client->GetId());

				Net::Api api("Auth_2nd_Create");

				api << client->GetAccountName() << packet.Password;

				const Net::Answer result = api.RequestApi();

				if (result.errorCode == 0) 
				{
					Lobby::Protocol::SecondPasswordCreated sendPacket;

					sendPacket.Result = static_cast<Lobby::Protocol::SecondPasswordCreated::Results>(result.errorCode);

					client->MakeSocketAsyncWriteSerializable(sendPacket);
				}
			});

			g_gameServer->Proxy.PacketHandler.Connect([](std::shared_ptr<ClientProxyLobby>& client, Lobby::Protocol::DeleteSecondPassword& packet)
			{
				Logger::GetInstance().Info("fwPacketListener :: userId@{0} :: protocol@DeleteSecondPassword", client->GetId());
				
				Net::Api api("Auth_2nd_Delete");

				api << client->GetAccountName() << packet.Password;

				const Net::Answer result = api.RequestApi();

				if (result.errorCode == 0) 
				{
					Lobby::Protocol::SecondPasswordDeleted sendPacket;

					sendPacket.Result = static_cast<Lobby::Protocol::SecondPasswordDeleted::Results>(result.errorCode);

					client->MakeSocketAsyncWriteSerializable(sendPacket);
				}
			});

			g_gameServer->Proxy.PacketHandler.Connect([](std::shared_ptr<ClientProxyLobby>& client, Lobby::Protocol::ModifySecondPassword& packet)
			{
				Logger::GetInstance().Info("fwPacketListener :: userId@{0} :: protocol@ModifySecondPassword", client->GetId());

				Net::Api api("Auth_2nd_Modify");

				api << client->GetAccountName() << packet.Password << packet.NewPassword;

				const Net::Answer result = api.RequestApi();

				if (result.errorCode == 0)
				{
					Lobby::Protocol::SecondPasswordModified sendPacket;

					sendPacket.Result = static_cast<Lobby::Protocol::SecondPasswordModified::Results>(result.errorCode);

					client->MakeSocketAsyncWriteSerializable(sendPacket);
				}
					
			});

			g_gameServer->Proxy.PacketHandler.Connect([](std::shared_ptr<ClientProxyLobby>& client, Lobby::Protocol::SignInSecondPassword& packet)
			{
				Logger::GetInstance().Info("fwPacketListener :: userId@{0} :: protocol@SignInSecondPassword", client->GetId());

				Net::Api api("Auth_2nd_Sign");

				api << client->GetAccountName() << packet.Password;

				const Net::Answer result = api.RequestApi();

				Lobby::Protocol::SecondPasswordSignedIn sendPacket;

				sendPacket.Result = static_cast<Lobby::Protocol::SecondPasswordSignedIn::Results>(result.errorCode);

				client->MakeSocketAsyncWriteSerializable(sendPacket);

			});

			g_gameServer->Proxy.PacketHandler.Connect([](std::shared_ptr<ClientProxyLobby>& client, Lobby::Protocol::Terminate& packet)
			{
				Logger::GetInstance().Info("fwPacketListener :: userId@{0} :: protocol@Terminate", client->GetId());
				//Gotta safely disconect the user. 
				Net::Api api("Terminate");

				api << client->GetAccountName();

				const Net::Answer result = api.RequestApi();

				Lobby::Protocol::Terminate sendPacket;

				sendPacket.Result = Lobby::Protocol::Terminate::Results::Ok;

				client->MakeSocketAsyncWriteSerializable(sendPacket);

				g_gameServer->Proxy.ClientRegistry.DropClient(client);
			});

			g_gameServer->Proxy.PacketHandler.Connect([](std::shared_ptr<ClientProxyLobby>& client, Lobby::Protocol::CreateCharacter& packet)
			{
				Logger::GetInstance().Info("fwPacketListener :: userId@{0} :: protocol@CreateCharacter", client->GetId());
					
				bool checkLicense = true;
				if (XRated::Constants::IsDefaultClass(packet.ClassType) == false)
					checkLicense = client->HasLicense(packet.ClassType);

				Net::Api api("CreateCharacter");

				api << client->GetAccountName() << packet.Name << static_cast<int>(packet.ClassType) << checkLicense;

				const Net::Answer result = api.RequestApi();

				Lobby::Protocol::CreateCharacter sendPacket;

				sendPacket.Result = static_cast<Lobby::Protocol::CreateCharacter::Results>(result.errorCode);

				if (result.errorCode == 0) 
				{
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
						
					for (auto y : result.resultObject["characterLicenses"].get<json>()) // [{"stageHash":19999,"accessLevel":1,"difficulty": 1}]
					{ 
						sendPacket.CharacterInfo.Licenses.push_back(XRated::StageLicense(y["stageHash"].get<uint32>(), y["accessLevel"].get<uint16>(), y["difficulty"].get<uint8>()));
					}

					for (auto y : result.resultObject["items"].get<json>()) 
					{
						XRated::ItemSlot slot;
						slot.Position.Bag = y["bagNumber"].get<uint8>(); // equipment slots at
						slot.Position.Position = y["positionNumber"].get<uint8>();
						slot.Stacked = 1; // equipments cannot be stacked
						slot.Id = y["itemHash"].get<uint32>();
						slot.instanceEx.Instance = y["instance"].get<int64>();
						slot.instanceEx.ExpireDate.Parse(y["itemExpire"].get<std::wstring>());
						sendPacket.CharacterInfo.Equipments.push_back(slot);
					}

					client->m_characters.push_back(sendPacket.CharacterInfo);
				}

				client->MakeSocketAsyncWriteSerializable(sendPacket);
			});

			g_gameServer->Proxy.PacketHandler.Connect([](std::shared_ptr<ClientProxyLobby>& client, Lobby::Protocol::DeleteCharacter& packet)
			{
				Logger::GetInstance().Info("fwPacketListener :: userId@{0} :: protocol@DeleteCharacter", client->GetId());

				if (!client->IsAuthenticated()) 
				{
					Logger::GetInstance().Error(L"Non authorized user trying to delete a AccountName: {0}, characterName: {1}", client->GetAccountName(), packet.Name);
					
					g_gameServer->Proxy.ClientRegistry.DropClient(client);
					return;
				}

				if (!client->DoesCharacterExist(packet.Name)) 
				{
					Logger::GetInstance().Error(L"User is trying to delete a characterName that does not exist. AccountName: {0}, characterName: {1}", client->GetAccountName(), packet.Name);
					
					g_gameServer->Proxy.ClientRegistry.DropClient(client);
					return;
				}

				Net::Api api("DeleteCharacter");

				api << packet.Name;

				const Net::Answer result = api.RequestApi();

				Lobby::Protocol::DeleteCharacter sendPacket;

				sendPacket.Result = static_cast<Lobby::Protocol::DeleteCharacter::Results>(result.errorCode);
				if (result.errorCode == 0) 
				{
					sendPacket.DeletedCharacter = packet.Name;
					if (!client->DeleteCharacter(packet.Name)) 
					{
						Logger::GetInstance().Error(L"Could not Delete the specified characterName AccountName: {0}, characterName: {1}", client->GetAccountName(), packet.Name);
						
						g_gameServer->Proxy.ClientRegistry.DropClient(client);
					}
				}

				client->MakeSocketAsyncWriteSerializable(sendPacket);
			});

			g_gameServer->Proxy.PacketHandler.Connect([](std::shared_ptr<ClientProxyLobby>& client, Lobby::Protocol::SaveKeySetting& packet)
			{
				Logger::GetInstance().Info("fwPacketListener :: userId@{0} :: protocol@SaveKeySetting", client->GetId());

				Net::Api api("SaveKeySetting");

				api << packet.Account;

				const Net::Answer result = api.RequestPost(StringUtil::ToASCII(packet.Keycodes));
				if (result.errorCode != 0) 
				{
					//key save error;
					return;
				}
			});

			g_gameServer->Proxy.PacketHandler.Connect([](std::shared_ptr<ClientProxyLobby>& client, Lobby::Protocol::Alive& packet)
			{
				Logger::GetInstance().Info("fwPacketListener :: userId@{0} :: protocol@Alive", client->GetId());

				Lobby::Alive::AliveData answer(packet.Index,packet.Value1,packet.Value2,packet.Value3);

				client->m_isWaitingOnPing = true;
				// client->SetAliveAsLastTickAlivePing(answer);
			});

			g_gameServer->Proxy.PacketHandler.Connect([](std::shared_ptr<ClientProxyLobby>& client, Lobby::Protocol::SelectCharacter& packet)
			{
				Logger::GetInstance().Info("fwPacketListener :: userId@{0} :: protocol@SelectCharacter", client->GetId());

				Net::Api api("SelectCharacter");
				//api << Config::GetInstance().m_ServerName;
				api << client->GetAccountName();
				api << packet.CharacterName;

				auto result = api.RequestApi();

				Lobby::Protocol::SelectCharacter sendPacket;

				sendPacket.Result = static_cast<Lobby::Protocol::SelectCharacter::Results>(result.errorCode);

				if (result.errorCode == 0 
					&& client->DoesCharacterExist(packet.CharacterName)
					&& client->SetSelectedCharacter(packet.CharacterName))
				{
					sendPacket.SelectedCharacter = StringUtil::ToUnicode(result.resultObject["characterName"].get<std::string>());
					sendPacket.CharacterStates = client->GetCharacterStateFlags();
					sendPacket.CharacterStates.IsPcRoom = 1; // AllM also sets this for some reason.
					sendPacket.CharacterStates.IsAdmin = 1;
					//after checking if the user is admin or not we gonna update the flags.
					client->SetCharacterStateFlags(sendPacket.CharacterStates);
					//Family check if there is a family to be joined as a guest will come in here

					//aswell as making sure to send the achievement server to the client in here.
					{
						Net::Api achivementApi("FindAchievementServer");
						achivementApi << packet.CharacterName;
						achivementApi.GetAsync([&](Net::Answer& result) 
						{
							Lobby::Protocol::AchievementServerAssigned packetAchievement;
							packetAchievement.Result = Lobby::Protocol::AchievementServerAssigned::Result::Ok;
							if (result.errorCode == 0) 
							{
								if (client->DoesCharacterExist(packet.CharacterName)
									&& !result.resultObject.is_null()) 
								{
									packetAchievement.ServerName = StringUtil::ToUnicode(result.resultObject["serverName"].get<std::string>());
									packetAchievement.serverAddress.Ip = result.resultObject["clientIp"].get<std::string>();
									packetAchievement.serverAddress.Port = result.resultObject["clientPort"].get<uint16>();
								}

								client->MakeSocketAsyncWriteSerializable(packetAchievement);
							}
						});
					}
				}
				else
				{
					Logger::GetInstance().Warn(L"Error Selecting user's={0}, characterName={1}", client->GetAccountName(), packet.CharacterName);
				}

				client->MakeSocketAsyncWriteSerializable(sendPacket);
				//send terminate in case this fails. Either users doesnt have the character or the api returned an error.
			});

			g_gameServer->Proxy.PacketHandler.Connect([](std::shared_ptr<ClientProxyLobby>& client, Lobby::Protocol::DeselectCharacter& packet)
			{
				Logger::GetInstance().Info("fwPacketListener :: userId@{0} :: protocol@DeselectCharacter", client->GetId());

				Net::Api api("DeselectCharacter");

				api << client->GetCharacterName();

				const Net::Answer result = api.RequestApi();

				Lobby::Protocol::DeselectCharacter sendPacket;

				if (result.errorCode == 0)
				{ 
					sendPacket.Result = Lobby::Protocol::DeselectCharacter::Results::Ok;
				}
				else
				{
					sendPacket.Result = Lobby::Protocol::DeselectCharacter::Results::NoResponse;

					Logger::GetInstance().Warn(L"Could not deselect users={0}, character={1}.", client->GetId(), client->GetCharacterName());
				}

				client->MakeSocketAsyncWriteSerializable(sendPacket);
			});

			g_gameServer->Proxy.PacketHandler.Connect([](std::shared_ptr<ClientProxyLobby>& client, Lobby::Protocol::ListSquareStatus& packet)
			{
				Logger::GetInstance().Info("fwPacketListener :: userId@{0} :: protocol@ListSquareStatus", client->GetId());
				if (client->HasSelectedACharacter()) 
				{
					Net::Api api("ListSquareStatus");

					auto result = api.RequestApi();

					Lobby::Protocol::ListSquareStatus sendPacket;

					if (result.errorCode == 0) 
					{
						if (result.resultObject.size() > 0) 
						{
							for (auto& x : result.resultObject) 
							{
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
						{
							Logger::GetInstance().Warn("There are no square stages to be listed");
						}
					}

					sendPacket.Result = static_cast<Lobby::Protocol::ListSquareStatus::Results>(result.errorCode);

					client->MakeSocketAsyncWriteSerializable(sendPacket);
				}
				else
				{
					client->Error(ErrorLevel::Curious, L"");
				}
			});

			g_gameServer->Proxy.PacketHandler.Connect([](std::shared_ptr<ClientProxyLobby>& client, Lobby::Protocol::JoinSquare& packet)
			{
				Logger::GetInstance().Info("fwPacketListener :: userId@{0} :: protocol@JoinSquare", client->GetId());
				if (client->IsAuthenticated() && client->HasSelectedACharacter()) 
				{
					Lobby::Protocol::Join sendPacket;
					sendPacket.Result = Lobby::Protocol::Join::Results::NoResponse;
					Net::Api api("JoinSquare");
					json j;
					j["characterName"] = StringUtil::ToASCII(client->GetCharacterName());
					j["squareName"] = StringUtil::ToASCII(packet.SquareName);
					api.PostAsync([&](Net::Answer& answer) 
					{
						sendPacket.Result = static_cast<Lobby::Protocol::Join::Results>(answer.errorCode);
						if (answer.errorCode == 0 && !answer.resultObject.is_null()) {
							sendPacket.KeyCode = std::to_string(answer.resultObject["reservationSerial"].get<int>());
							sendPacket.Port = answer.resultObject["serverPort"].get<uint16>();
							sendPacket.ServerIp = answer.resultObject["serverAddress"].get<std::string>();
						}
						else
						{
							Logger::GetInstance().Info("User[{0}] JoinSquare call failed.", client->GetId());
						}

					}, j);

					client->MakeSocketAsyncWriteSerializable(sendPacket);
				}
				else
				{
					Logger::GetInstance().Info("User[{0}] is trying to call JoinSquare unauthenticated.", client->GetId());
				}
			});
		});
}
