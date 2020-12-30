#include <StageServer/User/UserManager.h>
#include <Info/Info.h>
#include <Network/Api/Api.h>
#include <StageServer/User/User.h>
#include <StageServer/StageServerProtocol/StageServerProtocol.h>
#include <StageServer/Room/RoomManager.h>
#include <StageServer/Room/Room.h>

namespace Lunia {
	namespace XRated {
		namespace StageServer
		{
			UserManager& UserManager::GetInstance()
			{
				static UserManager m_Instance;
				return m_Instance;
			}

			UserManager::~UserManager() {}

			UserManager::UserManager() {}

			UserSharedPtr UserManager::MakeUser(asio::ip::tcp::socket& socket) {
				AutoLock lock(m_tempUsersMutex);
				m_tempUsers[m_curUserId] = std::make_shared<User>(std::move(socket), m_curUserId); //moving memory to inside of the vector and keeping the pointer ownership.
				OnUserConnected(m_tempUsers[m_curUserId]);
				return m_tempUsers[m_curUserId++];
			}
			
			void UserManager::RemoveUser(const uint64& userSerial) {
				AutoLock _l(m_usersMutex);
				this->RemoveUser(userSerial, _l);
			}

			UserSharedPtr UserManager::GetUserByConnectionId(const uint32& userId){
				AutoLock _l(m_tempUsersMutex);
				if (m_tempUsers.find(userId) != m_tempUsers.end())
					return m_tempUsers[userId];
				Logger::GetInstance().Error("Could not find the specified TempUser@{0}", userId);
				return nullptr;
			}

			UserSharedPtr UserManager::GetUserBySerial(const uint64& userSerial)
			{
				AutoLock _l(m_usersMutex);
				if (m_users.find(userSerial) != m_users.end())
					return m_users[userSerial];
				Logger::GetInstance().Error("Could not find the specified user@{0}", userSerial);
				return nullptr;
			}

			void UserManager::RemoveUser(const uint64& userSerial, AutoLock& _l) {

				//AutoLock lock(m_usersMutex);
				/*
					This second lock causes Thread lock. The thread is already locked by the lock on RemoveUser(user)
					then it cant go past the lock above because the m_usersMutex is locked so it get stuck.
				*/
				if (m_users.find(userSerial) == m_users.end()) {
					Logger::GetInstance().Error("User={0} not found", userSerial);
					return;
				}
				AutoLock lock(m_users[userSerial]->mtx);//Lock the user to make sure it's sync.
				m_users[userSerial]->CloseSocket();
				OnUserDisconnected(userSerial);
				m_users.erase(userSerial);
			}
			/*
			bool UserManager::Auth(UserSharedPtr& user, const json& data) {
				
				//After this is all done and good we have to send this user to the "Room" responsible for the stage it'll be joining. but for now since we dont have any room
				//anything really i'll just "fake it"
				//Load
				{
					Protocol::LoadEnd sendPacket;
					sendPacket.charName = user->GetCharacterName();
					sendPacket.progress = 0;
					user->Send(sendPacket);
				}
				//RoomJoined
				{
					//Stage
					{
						Protocol::Stage sendPacket;
						sendPacket.charactername = user->GetCharacterName();
						sendPacket.targetStage = user->m_CurrentStage;
						user->Send(sendPacket);
					}
					//QuestManager
					{
						Net::Api api("Quest/WorkingList");
						api << user->GetCharacterName();
						api.GetAsync(
							[&](const Net::Answer& result) {
								if (result.errorCode == 0) {

									Protocol::Quest::WorkingList sendPacket; // even with no quests it has to be sent.
									for (auto& x : result.resultObject) {
										XRated::Quest quest;
										quest.Id = x["questHash"].get<uint32>();
										quest.ExpiredDate.Parse(x["expiredDate"].get<std::string>());
										quest.CurrentState = x["currentState"].get<uint8>();
										quest.Params.push_back(x["param1"].get<uint32>());
										quest.Params.push_back(x["param2"].get<uint32>());
										quest.Params.push_back(x["param3"].get<uint32>());
									}
									user->Send(sendPacket);
								}
								else
									Logger::GetInstance().Error(L"Could not parse WorkingQuests for the user@{0}", user->GetCharacterName());
							});
					}
					//FamilyManager // Im ignoring this for now. Let's do it later.
					if (1 == 0) {
						Net::Api api("Family/JoinedInfo");
						api << user->GetCharacterName();
						api.GetAsync(
							[&](const Net::Answer& result) {
								if (result.errorCode == 0) {

									Protocol::Family::Info sendPacket;
									for (auto& x : result.resultObject) {
									}
									user->Send(sendPacket);
								}
								else
									Logger::GetInstance().Error(L"Could not parse FamilyInfo for the user@{0}", user->GetCharacterName());
							});
					}
					//Load
					{
						Protocol::LoadEnd sendPacket;
						sendPacket.charName = user->GetCharacterName();
						sendPacket.progress = 1;
						user->Send(sendPacket);
					}
				}
				//EndOfRoomJoined
				{
					Protocol::CreatePlayer createplayer;
					createplayer.playerserial = user->GetSerial();
					createplayer.classtype = user->m_PlayerData.type;
					createplayer.charactername = user->GetCharacterName();//pPlayerData->BaseCharacter.BaseObject.Name;
					createplayer.level = user->m_PlayerData.level;//pPlayerData->BaseCharacter.Level;
					createplayer.pvpLevel = user->m_PlayerData.pvpLevel;//pPlayerData->PvpLevel;
					createplayer.warLevel = user->m_PlayerData.warLevel;//pPlayerData->WarLevel;
					createplayer.storedLevel = user->m_PlayerData.storedLevel;//pPlayerData->StoredLevel;
					createplayer.rebirthCount = user->m_PlayerData.rebirthCount;//pPlayerData->RebirthCount;
					createplayer.ladderPoint = user->m_PlayerData.ladderPoint;//pPlayerData->LadderPoint;
					createplayer.ladderMatchCount = user->m_PlayerData.ladderMatchCount;//pPlayerData->LadderMatchCount;
					createplayer.ladderWinCount = user->m_PlayerData.ladderWinCount;//pPlayerData->LadderWinCount;
					createplayer.ladderLoseCount = user->m_PlayerData.ladderLoseCount;//pPlayerData->LadderLoseCount;
					createplayer.achievementScore = user->m_PlayerData.achievementScore;//pPlayerData->achievementScore; // 3.1 by Robotex
					createplayer.addedSkillPointByRebirth = user->m_PlayerData.storedSkillPoint;//pPlayerData->StoredSkillPoint;
					createplayer.position = float3(1200, 0, 610);//pPlayerData->BaseCharacter.BaseObject.Position;
					createplayer.direction = float3(0, 0, -1);//pPlayerData->BaseCharacter.BaseObject.Direction;
					createplayer.hp = 1000;//pPlayerData->BaseCharacter.Hp;
					createplayer.team = 1;//pPlayerData->BaseCharacter.Team;

					createplayer.shopping = true;
					createplayer.stageLicenses = user->m_StageLicenses;

					createplayer.lives = static_cast<uint8>(user->m_PlayerData.life);
					createplayer.bonusLife = static_cast<uint8>(user->m_PlayerData.bonusLife);
					//createplayer.stateflags = pPlayerData->BaseCharacter.States;
					createplayer.lastRebirthDateTime = user->m_PlayerData.lastRebirthDateTime;//pPlayerData->LastRebirthDateTime;
					createplayer.partyChannelName = L"";//partyChannelName;
					createplayer.eventExpFactor = 1.0;
					user->Send(createplayer);
				}
				return true;
			}
				*/

			bool UserManager::AuthenticateUser(const uint32& userId, const json& result)
			{
				auto userSerial = result["charactersId"].get<uint64>();
				if (userSerial == 0)
					Logger::GetInstance().Exception("UserManager::AuthenticateUser() not a valid UserSerial={0}", userSerial);
				AutoLock l_(m_usersMutex);
				if (m_users.find(userSerial) != m_users.end()) { //looking to see if the userSerial is already listed on our m_Users
					Logger::GetInstance().Error("UserManager::AuthenticateUser() already registred user={0}.", userSerial);
					return false;
				} // Duplicated user
				if (m_tempUsers.find(userId) == m_tempUsers.end()) {
					// this is kind of weird. How would the user not be found on the authenticated list and at the same time not in the tempList?
					Logger::GetInstance().Exception("UserManager::AuthenticateUser() requested user={0} doesnt exist.", userSerial);
					return false;
				}
				{
					AutoLock lock(m_tempUsers[userId]->mtx); // making sure the user will remain sync while we work on it.
					m_users[userSerial] = std::move(m_tempUsers[userId]); m_tempUsers.erase(userId);
					m_users[userSerial]->SetSerial(userSerial);
				}
				return m_users[userSerial]->AuthConnection(result);
			}

			void UserManager::RoomAuth(const UserSharedPtr& user)
			{
				//RoomManagerInstance()
			}

			//Global Singleton
			UserManager& UserManagerInstance() {
				return UserManager::GetInstance();
			};
		}
	}
}
