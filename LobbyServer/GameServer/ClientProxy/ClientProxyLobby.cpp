#include "ClientProxyLobby.h"
#include <LobbyServer/LobbyProtocol/LobbyProtocol.h>
#include <LobbyServer/Main.h>
namespace Lunia {
	namespace XRated {
		ClientProxyLobby::ClientProxyLobby(asio::ip::tcp::socket&& socket) : ClientProxy(std::move(socket))
		{

		}; /* WARN socket is moved a shit ton of times here */

		void ClientProxyLobby::SetAsAuthenticated()
		{
			this->m_isAuthenticated = true;
		}

		bool ClientProxyLobby::IsAuthenticated() const
		{
			return this->m_isAuthenticated;
		}

		void ClientProxyLobby::SetHasSecondPasswordAuthentication()
		{
			this->m_hasSecondPasswordAuthentication = true;
		}

		bool ClientProxyLobby::HasSecondPasswordAuthentication() const
		{
			return this->m_hasSecondPasswordAuthentication;
		}

		bool ClientProxyLobby::Ping()
		{
			Lobby::Protocol::Alive packet;

			this->m_isWaitingOnPing = false;

			// Doesnt seem to be needing values at all?!

			/*
				packet.Index = 0;
				packet.Value1 = 0;
				packet.Value2 = 0;
				packet.Value3 = 0;
			*/

			this->MakeSocketAsyncWriteSerializable(packet);

			return true;
		}
		void ClientProxyLobby::CharacterListed(const ClientProxyLobbySharedPtr& client, Net::Answer& answer)
		{
			if (answer.errorCode == 0) {
				//AccountInfo
				{
					auto& account = answer.resultObject.at("accountInfo");
					account.at("characterSlots").get_to(client->m_numOfCharacterSlots);
					account.at("accountLicenses").get_to(client->m_accountLicense);
					//Lock user's account if second password is set 
					if (account.at("secondPassword").get<bool>())
						client->SetHasSecondPasswordAuthentication();
					else
						g_gameServer->Proxy.ClientRegistry.AuthenticateClient(client);
				}



				{
					Lobby::Protocol::CharacterSlots sendPacketCharacterSlots;

					sendPacketCharacterSlots.NumberOfSlots = client->m_numOfCharacterSlots;

					for (int i = 0; i < 16; i++) {
						if (client->m_accountLicense & (1 << i))
							sendPacketCharacterSlots.CharacterLicenses.push_back(i);
					};

					client->MakeSocketAsyncWriteSerializable(sendPacketCharacterSlots);
				}

				{
					for (auto& characterInfo : answer.resultObject.at("charactersInfo")) {
						client->m_characters.push_back(XRated::LobbyPlayerInfo());
						XRated::LobbyPlayerInfo& info = client->m_characters.back();
						{
							auto& x = characterInfo.at("info");
							x.at("characterName").get_to(info.CharacterName);
							x.at("id").get_to(info.CharacterSerial);
							x.at("id").get_to(info.VirtualIdCode);
							info.ClassType = static_cast<XRated::Constants::ClassType>(x.at("classNumber").get<int>());
							x.at("stageLevel").get_to(info.Level);
							x.at("stageExp").get_to(info.Exp);
							x.at("pvpLevel").get_to(info.PvpLevel);
							x.at("pvpExp").get_to(info.PvpExp);
							x.at("warLevel").get_to(info.WarLevel);
							x.at("warExp").get_to(info.WarExp);
							info.StateFlags = static_cast<XRated::CharacterStateFlags>(x.at("instantStateFlag").get<uint32>());
						}

						{
							auto& cr = characterInfo.at("characterrebirth");
							cr.at("rebirthCount").get_to(info.RebirthCount);
							cr.at("storedLevel").get_to(info.StoredLevel);
						}

						for (auto& item : characterInfo.at("stagelicenses")) { //[{"stageHash":19999,"accessLevel":1,"difficulty": 1}]
							info.Licenses.push_back(
								XRated::StageLicense(item.at("stageGroupHash").get<uint32>(), item.at("accessLevel").get<uint16>()));
						}

						for (auto& item : characterInfo.at("items")) {
							XRated::ItemSlot slot;
							item.at("bagNumber").get_to(slot.Position.Bag);
							item.at("positionNumber").get_to(slot.Position.Position);
							slot.Stacked = 1; // equipments cannot be stacked
							item.at("itemHash").get_to(slot.Id);
							item.at("instance").get_to(slot.InstanceEx.Instance);
							item.at("itemExpire").get_to(slot.InstanceEx.ExpireDate);
							info.Equipments.push_back(std::move(slot));
						}
					}
					Lobby::Protocol::ListCharacter sendPacket;
					sendPacket.Characters = client->m_characters;
					client->MakeSocketAsyncWriteSerializable(sendPacket);
				}

			}
			else
			{
				/*
					Error code is not 0
				*/
				Logger::GetInstance().Warn("[{}] Error requesting for the user's character list", client->GetId());
			}
		}
		void ClientProxyLobby::AchivementServerFound(const ClientProxyLobbySharedPtr& client, Net::Answer& answer)
		{
			Lobby::Protocol::AchievementServerAssigned packetAchievement;
			packetAchievement.Result = Lobby::Protocol::AchievementServerAssigned::Result::Ok;
			if (answer.errorCode == 0)
			{
				if (DoesCharacterExist(GetCharacterName())
					&& !answer.resultObject.is_null())
				{
					answer.get_to("serverName", packetAchievement.ServerName);
					answer.get_to("clientIp", packetAchievement.serverAddress.Ip);
					answer.get_to("clientPort", packetAchievement.serverAddress.Port);
				}
				client->MakeSocketAsyncWriteSerializable(packetAchievement);
			}
		}
	}
}