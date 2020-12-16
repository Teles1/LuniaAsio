#include "UserRegistry.h"
#include "../Core/Utils/InitFunction.h"
#include <Core/Utils/ThreadPool.h>
#include "./Network/Api/Api.h"

namespace Lunia {
	namespace Net
	{
		Lobby::UserSharedPtr UserRegistry::MakeUser(asio::ip::tcp::socket& socket) {
			Lobby::UserSharedPtr user(new Lobby::User(m_curUserId, std::move(socket)));
			Lobby::UserWeakPtr userWeak{ user };

			m_users.push_back(user);

			m_usersByUserId[m_curUserId] = userWeak;
			m_usersByUserId[m_curUserId] = userWeak;
			m_autorizedUsersByUserId[m_curUserId] = userWeak;

			user->SetId(m_curUserId);

			OnUserConnected(user);

			m_curUserId--;

			return user;
		}

		/* 
			Pending removal on the database and cleanup.
		*/
		void UserRegistry::RemoveUser(Lobby::UserSharedPtr& user) {
			//m_users.erase(user->GetId());
			AutoLock _l(m_usersMutex);
			this->RemoveUser(user, _l);
		}
		/*
			Pending removal on the database and cleanup.
		*/
		void UserRegistry::RemoveUser(Lobby::UserSharedPtr& user, AutoLock& /*No need to touch this because the scope is gonna be locked automatically*/) {
			/*
				Key pending aproval for this change.
			*/

			OnUserDisconnected(user);

			m_usersByUserId.erase(user->GetId());

			if (user->IsAuthenticated())
				m_autorizedUsersByUserId.erase(user->GetId());

			for (auto it = m_users.begin(); it < m_users.end(); it++)
			{
				if ((*it)->GetId() == user->GetId())
				{
					(*it)->CloseSocket();

					m_users.erase(it);

					break;
				}
			}
		}

		void UserRegistry::AuthenticateUser(Lobby::UserSharedPtr& user)
		{
			if (user)
			{
				user->SetIsAuthenticated();

				OnUserAuthenticated(user);
			}
		}

		Lobby::UserSharedPtr UserRegistry::GetUserByUserId(uint32 userId) {
			AutoLock _l(m_usersMutex);
			auto ptr = Lobby::UserSharedPtr();
			if (m_usersByUserId.find(userId) != m_usersByUserId.end())
			{
				ptr = m_usersByUserId[userId].lock();
			}

			return ptr;
		}
		UserRegistry::UserRegistry(const uint32& timeout) {
			m_timeOutTimer = timeout;
			m_keepAliveThread = std::thread([&] {
				while (m_keepAliveLoop) 
				{
					if (m_usersByUserId.size() > 0)
						NextAlivePingForAll();

					std::unique_lock<std::mutex> _l(m_conditionalVar_lock);
					if (m_conditionalVar.wait_for(_l, std::chrono::milliseconds(m_timeOutTimer), [&] { return !m_keepAliveLoop; }))
						Logger::GetInstance().Info("Keep Alive Loop has been suceffully disabled");
				}
			});
		}
		void UserRegistry::NextAlivePingForAll()
		{
			AutoLock _l(m_usersMutex);
			for (auto& user : m_users) {
				if (user->IsWaitingOnAlivePing()) 
				{
					user->SatisfyAlivePingWait();
				}
				else 
				{
					Logger::GetInstance().Info("Invalid Alive Processing. Terminating user's connection => {0}", user->GetId());

					this->RemoveUser(user, _l);
				}
			}
		}
	}

	static utils::InitFunction initFunction([]()
		{
			Net::UserRegistry::GetInstance().OnUserConnected.Connect([](const Lobby::UserSharedPtr& user)
				{
					Logger::GetInstance().Info("UserRegistry :: OnUserConnected :: userId@{0}", user->GetId());
				});

			Net::UserRegistry::GetInstance().OnUserDisconnected.Connect([](const Lobby::UserSharedPtr& user)
				{
					Logger::GetInstance().Info("UserRegistry :: OnUserDisconnected :: userId@{0}", user->GetId());

				});


			Net::UserRegistry::GetInstance().OnUserAuthenticated.Connect([](const Lobby::UserSharedPtr& user)
				{
					Logger::GetInstance().Info("UserRegistry :: OnUserAuthenticated :: userId@{0}", user->GetId());
				
					Net::Api apiRequest("ListCharacters");

					apiRequest << user->GetAccountName();

					auto requestResult(apiRequest.RequestApi());

					if (requestResult.errorCode == 0) 
					{

						//CharacterLicenses
						{
							user.get()->m_NumberOfSlots = requestResult.resultObjet["characterSlots"].get<uint8>();
							user->m_AccountLicenses = requestResult.resultObjet["accountsLicense"].get<int32>();
						}

						//Lock user's account if second password is set 
						//if (!result.resultObjet["secondPassword"].get<bool>())
							//user->SetIsAuthenticated();

						{
							Lobby::Protocol::CharacterSlots toSendPacket;

							toSendPacket.NumberOfSlots = user->m_NumberOfSlots;

							for (int i = 0; i < 16; i++) {
								if (static_cast<bool>(user->m_AccountLicenses & (1 << i)))
									toSendPacket.CharacterLicenses.push_back(i);
							}

							user->Send(toSendPacket);
						}

						if (!requestResult.resultObjet["characters"].is_null()) {
							for (auto y : requestResult.resultObjet["characters"]) {
								user->m_Characters.push_back(XRated::LobbyPlayerInfo());

								XRated::LobbyPlayerInfo& info = user->m_Characters.back();

								info.CharacterName   = StringUtil::ToUnicode(y["characterName"].get<std::string>());
								info.CharacterSerial = y["id"].get<int64>();
								info.VirtualIdCode   = y["id"].get<uint32>();
								info.ClassType		 = static_cast<XRated::Constants::ClassType>(y["classNumber"].get<int>());
								info.Level			 = y["stageLevel"].get<uint16>();
								info.Exp			 = y["stageExp"].get<uint32>();
								info.PvpLevel		 = y["pvpLevel"].get<uint16>();
								info.PvpExp			 = y["pvpExp"].get<uint32>();
								info.WarLevel		 = y["warLevel"].get<uint16>();
								info.WarExp			 = y["warExp"].get<uint32>();
								info.StateFlags		 = static_cast<XRated::CharacterStateFlags>(y["instantStateFlag"].get<int>());
								info.RebirthCount	 = y["characterRebirth"]["rebirthCount"].get<uint16>();
								info.StoredLevel	 = y["characterRebirth"]["storedLevel"].get<uint16>();

								for (auto y : y["characterLicenses"].get<json>()) { //[{"stageHash":19999,"accessLevel":1,"difficulty": 1}]
									info.Licenses.push_back(XRated::StageLicense(y["stageHash"].get<uint32>(), y["accessLevel"].get<uint16>(), y["difficulty"].get<uint8>()));
								}

								for (auto y : y["items"].get<json>()) {
									XRated::ItemSlot slot;

									slot.Position.Bag        = y["bagNumber"].get<uint8>(); // equipment slots at
									slot.Position.Position   = y["positionNumber"].get<uint8>();
									slot.Stacked		     = 1; // equipments cannot be stacked
									slot.Id				     = y["itemHash"].get<uint32>();
									slot.instanceEx.Instance = y["instance"].get<int64>();
									slot.instanceEx.ExpireDate.Parse(StringUtil::ToUnicode(y["itemExpire"].get<std::string>()));
									
									info.Equipments.push_back(slot);
								}
							}
						}

						{
							Lobby::Protocol::ListCharacter toSendPacket;

							toSendPacket.Characters = user->m_Characters;

							user->Send(toSendPacket);
						}
					}
					else
					{
						Logger::GetInstance().Warn("[{}] Error requesting for the user's character list", user->GetId());
					}
				});
		});
}