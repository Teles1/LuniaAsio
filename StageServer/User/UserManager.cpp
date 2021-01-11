#include <StageServer/User/UserManager.h>
#include <Info/Info.h>
#include <StageServer/User/User.h>
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

			bool UserManager::IsEventQuestPeriod(const uint8& eventType)
			{
				auto iter = m_QuestEventStates.find(eventType);
				if (iter != m_QuestEventStates.end())
				{
					return m_QuestEventStates[eventType];
				}

				return false;
			}

			void UserManager::CheckAllEventQuestPeriod(UserSharedPtr user)
			{
				for (auto& qEvent : m_QuestEventStates) {
					if (qEvent.second) {
						Protocol::FromServer::QuestEvent::EnableQuestEvent sendPacket;
						sendPacket.EventQuestID = qEvent.first;
						sendPacket.Enable = true;
						sendPacket.EventExplain = L"Ala viado, nao tem implementacao :(";
						/*QuestEventExplains::const_iterator explainIter = questEventExplains.find(iter->first);
						if (explainIter != questEventExplains.end())
						{
							sendPacket.EventExplain = ServerLocale::ServerLocaleLoader::Instance().GetLocalizedText(explainIter->second.c_str(), user.GetUserLocale());
						}
						*/
						user->Send(sendPacket);
					}
				}
			}

			void UserManager::CheckAllNpcDropEventPeriod(UserSharedPtr user)
			{
				for (auto& nEvent : m_NpcDropEventStates) {
					if (nEvent.second) {
						Protocol::FromServer::NpcDropEvent::EnableNpcDropEvent sendPacket;
						sendPacket.EventID = nEvent.first;
						sendPacket.Enable = true;
						sendPacket.EventExplain = L"Teles, Implementa isso aqui cara";
						/*QuestEventExplains::const_iterator explainIter = questEventExplains.find(iter->first);
						if (explainIter != questEventExplains.end())
						{
							sendPacket.EventExplain = ServerLocale::ServerLocaleLoader::Instance().GetLocalizedText(explainIter->second.c_str(), user.GetUserLocale());
						}
						*/
						user->Send(sendPacket);
					}
				}
			}

			std::wstring UserManager::GetLastTextBoardMsg() const
			{
				return m_LastTextBoardMsg;
			}

			bool UserManager::IsScriptEventNow(const uint32& eventId) const
			{
				if (m_ScriptEventMap.find(eventId) != m_ScriptEventMap.end())
					return true;
				return false;
			}

			bool UserManager::DoesExist(const UserSharedPtr& user) const
			{
				AutoLock lock(m_usersMutex);
				if (m_users.find(user->GetSerial()) != m_users.end())
					return true;
				return false;
			}

			void UserManager::SendCurrentStageEvents(const UserSharedPtr& user) const
			{
			}

			void UserManager::StageChanged(UserSharedPtr user)
			{
				//communicators.PlayerStore.Close(user);
				//communicators.Trader.Cancel(user);
				if (user->IsFishing())
				{
					user->RemoveFishingUser();
				}
			}

			void UserManager::RequestValidGuildInfo(UserSharedPtr user)
			{
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

			UserSharedPtr UserManager::GetUserByName(const std::wstring& name)
			{
				AutoLock _l(m_usersMutex);
				for(auto& user : m_users)
					if(user.second->GetCharacterName() == name)
						return user.second;
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
				AutoLock lock(m_users[userSerial]->GetSyncObject());//Lock the user to make sure it's sync.
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
					AutoLock lock(m_tempUsers[userId]->GetSyncObject()); // making sure the user will remain sync while we work on it.
					m_users[userSerial] = std::move(m_tempUsers[userId]); m_tempUsers.erase(userId);
					m_users[userSerial]->SetSerial(userSerial);
				}
				return m_users[userSerial]->AuthConnection(result);
			}

			void UserManager::RoomAuth(const UserSharedPtr& user)
			{
				if (!RoomManagerInstance().RoomJoin(user->GetRoomIndex(), user, user->GetRoomPass())) {
					LoggerInstance().Error("user={0} failed to join room={1}", user->GetSerial(), user->GetRoomIndex());
					user->Terminate();
					return;
				}
				if (!user->IsConnected())
					user->Terminate();
				Net::Api request("Market.GetExpired"); //missing implementation
				request << user->GetCharacterName();
				request.GetAsync();
			}

			void UserManager::CurrentEvent::SendCurrentStageEvents(UserSharedPtr& user)
			{
				AutoLock lock(cs);

				for (CurrentEventInfos::const_iterator iter = infos.begin(); iter != infos.end(); ++iter)
				{
					uint32 stageGroupHash = StringUtil::ToInt(iter->second.first[0].c_str());
					uint16 level = StringUtil::ToInt(iter->second.first[1].c_str());

					StageLocation stageLocation(stageGroupHash, level);
					Database::Info::StageGroup* stageGroup = Database::DatabaseInstance().InfoCollections.StageGroups.Retrieve(stageLocation.StageGroupHash);
					if (!stageGroup)
					{
						LoggerInstance().Warn("UserManager::CurrentEvent::SendCurrentStageEvents - Invalid StageGroup : {}", stageGroupHash);
						return;
					}

					if (stageGroup->Stages.size() <= stageLocation.Level)
					{
						LoggerInstance().Warn("UserManager::CurrentEvent::SendCurrentStageEvents - Invalid StageInfo : {}, {}", stageGroupHash, level);
						return;
					}

					Database::Info::StageInfo* stageInfo = stageGroup->Stages[stageLocation.Level];
					if (!stageInfo)
					{
						LoggerInstance().Warn("UserManager::CurrentEvent::SendCurrentStageEvents - Invalid StageInfo : {}, {}", stageGroupHash, level);
						return;
					}

					uint32 lowRank = 0;
					uint32 highRank = 0;
					if (stageGroup->GuildStage)
					{
						lowRank = stageInfo->lowGuildRank;
						highRank = stageInfo->highGuildRank;
					}

					user->RaiseEvent(iter->second.first, lowRank, highRank, iter->second.second);
				}
			}

			void UserManager::CurrentEvent::AddCurrentEvent(const uint32& key, const Database::Info::EventUnit::ParamContainer& eventInfo, const DateTime& endTime)
			{
				AutoLock lock(cs);

				infos[key] = std::make_pair(eventInfo, endTime);
			}

			void UserManager::CurrentEvent::EraseEvent(const uint32& key)
			{
				AutoLock lock(cs);

				infos.erase(key);
			}

			//Global Singleton
			UserManager& UserManagerInstance() {
				return UserManager::GetInstance();
			};
}
	}
}
