#include "RoomUserManager.h"
#include <StageServer/User/User.h>
#include <Core/Utils/ConfigReader.h>
#include <mmsystem.h>
#include <StageServer/User/UserManager.h>
namespace Lunia {
	namespace XRated {
		namespace StageServer {
			RoomUserManager::RoomUserManager() : totalCnt(0), maxCnt(0), SaveTimeInMs(ConfigInstance().Get("SynchronizePlayerDataInMsec", 180000))
			{
				//mailAlarm.INTERVAL = ConfigInfoInstance().mailAlarmIntervalInSec;
				mailAlarm.Init();
			}

			RoomUserManager::RoomUserManager(const RoomUserManager&)
				: totalCnt(0), maxCnt(0)
			{
				//mailAlarm.INTERVAL = ConfigInfoInstance().mailAlarmIntervalInSec;
				mailAlarm.Init();
			}


			RoomUserManager::~RoomUserManager()
			{
				Clear();
			}


			void RoomUserManager::Clear()
			{
				AutoLock lock(cs);
				AutoLock updateLock(updateCS);

				nameUsers.clear();
				serialUsers.clear();
				cashItemViewList.clear();

				mailAlarm.Init();
			}

			void RoomUserManager::ClearNameUser()
			{
				AutoLock lock(cs);
				AutoLock updateLock(updateCS);

				nameUsers.clear();
			}

			void RoomUserManager::ClearSerialUser()
			{
				AutoLock lock(cs);
				AutoLock updateLock(updateCS);

				serialUsers.clear();
				cashItemViewList.clear();
			}

			void RoomUserManager::AddNameUser(const std::wstring& name, UserSharedPtr user)
			{
				AutoLock lock(cs);
				AutoLock updateLock(updateCS);

				nameUsers[name] = user;

				++totalCnt;

				if (maxCnt < nameUsers.size())
					maxCnt = nameUsers.size();
			}


			void RoomUserManager::AddSerialUser(unsigned int serial, UserSharedPtr user)
			{
				AutoLock lock(cs);
				AutoLock updateLock(updateCS);

				serialUsers[serial] = user;
			}


			bool RoomUserManager::DelNameUser(const std::wstring& name)
			{
				AutoLock lock(cs);
				AutoLock updateLock(updateCS);

				//Remove from mail alarm list
				std::wstring tmpName(name);
				StringUtil::ToLowerCase(tmpName);
				std::map<std::wstring, bool>::iterator iMail = mailAlarm.userlist.find(tmpName);
				if (iMail != mailAlarm.userlist.end())
					mailAlarm.userlist.erase(iMail);

				stringUserMap::iterator itr = nameUsers.find(name);
				if (itr == nameUsers.end())
					return false;

				nameUsers.erase(itr);

				return true;
			}


			bool RoomUserManager::DelSerialUser(unsigned int serial)
			{
				AutoLock lock(cs);
				AutoLock updateLock(updateCS);

				std::map< XRated::Serial, std::pair< uint16, uint16 > >::iterator iCashView = cashItemViewList.find(serial);
				if (iCashView != cashItemViewList.end()) {
					cashItemViewList.erase(iCashView);
					LoggerInstance().Info("[RoomUserManager::DelSerialUser] User removed from cashitemview list. ({})", (int)serial);
				}

				serialUserMap::iterator itr = serialUsers.find(serial);
				if (itr == serialUsers.end())
					return false;

				serialUsers.erase(itr);
				return true;
			}


			UserSharedPtr RoomUserManager::GetUser(const std::wstring& name)
			{
				AutoLock lock(cs);

				stringUserMap::iterator itr = std::find_if(nameUsers.begin(), nameUsers.end(), UserFinder(name));
				if (itr != nameUsers.end())
					return itr->second;

				return  NULL;
			}


			UserSharedPtr RoomUserManager::GetUser(unsigned int serial)
			{
				AutoLock lock(cs);

				serialUserMap::iterator itr = serialUsers.find(serial);
				if (itr != serialUsers.end())
					return itr->second;

				return  NULL;
			}


			bool RoomUserManager::IsEnableStage(const StageLicense& targetStage)
			{
				AutoLock lock(cs);

				//for(serialUserMap::iterator i = serialUsers.begin() ; i != serialUsers.end() ; ++i)
				for (stringUserMap::iterator i = nameUsers.begin(); i != nameUsers.end(); ++i)
					if (!i->second->IsAbleToJoinStage(targetStage))
						return false;

				return true;
			}


			bool RoomUserManager::IsJoinningUser()
			{
				AutoLock lock(cs);

				return nameUsers.size() != serialUsers.size();
			}


			void RoomUserManager::Update(float dt)
			{
				//AutoLock lock(cs);
				AutoLock lock(updateCS);

				DWORD nowTime = timeGetTime();
				for (stringUserMap::iterator i = nameUsers.begin(); i != nameUsers.end(); ++i) {
					i->second->Update(dt);
					i->second->AutoSaveUserInfo(nowTime, SaveTimeInMs);
				}

				//if ( ConfigInfoInstance().ServerInfo.isSquare == true)
				//	UpdateMailAlarm(dt);
			}

			void RoomUserManager::UpdateMailAlarm(float dt)
			{
				if (mailAlarm.Update(dt) == true) {
					if (mailAlarm.userlist.empty())
						return;

					Net::Api request("Mail.AlarmList", Net::Api::Methods::POST);
					for (std::map<std::wstring, bool>::iterator i = mailAlarm.userlist.begin(); i != mailAlarm.userlist.end(); ++i)
					{
						request << i->first;
						if (i->second == true)
						{
							UserSharedPtr user = GetUser(i->first);
							if (user != NULL)
							{
								if (user->GetMailCnt() == 0)
								{
									i->second = false;
								}
							}
						}
					}
					request.GetAsync(this, &RoomUserManager::MailAlarmed);
				}
			}

			void RoomUserManager::GetSpectatorNames(std::vector< std::wstring >& names)
			{
				names.clear();
				AutoLock lock(cs);
				/* spectators should get broadcast from game logic */
				{
					// TODO: you may optimize by "stringUserMap spectatorUsers;" member
					for (stringUserMap::iterator i = nameUsers.begin(); i != nameUsers.end(); ++i)
					{
						if (i->second->GetCharacterStateFlags().IsSpectator)
							names.push_back(i->second->GetName());
					}
				}
			}

			void RoomUserManager::BroadcastToSpectators(Serializer::ISerializable& value)
			{
				AutoLock lock(cs);
				/* spectators should get broadcast from game logic */
				{
					// TODO: you may optimize by "stringUserMap spectatorUsers;" member
					for (stringUserMap::iterator i = nameUsers.begin(); i != nameUsers.end(); ++i)
					{
						if (i->second->GetCharacterStateFlags().IsSpectator && i->second->IsLoaded())
							i->second->Send(value);
					}
				}
			}
			void RoomUserManager::BroadcastToSpectators(Serializer::ISerializable& value, const std::wstring& ignoreUserName)
			{
				AutoLock lock(cs);
				/* spectators should get broadcast from game logic */
				{
					// TODO: you may optimize by "stringUserMap spectatorUsers;" member
					for (stringUserMap::iterator i = nameUsers.begin(); i != nameUsers.end(); ++i)
					{
						if ((i->second->GetCharacterStateFlags().IsSpectator) && (i->second->IsLoaded()) && (ignoreUserName != i->second->GetName()))
							i->second->Send(value);
					}
				}
			}

			void RoomUserManager::BroadcastToSerialUsers(Serializer::ISerializable& value)
			{
				{
					AutoLock lock(cs);

					{
						for (serialUserMap::iterator i = serialUsers.begin(); i != serialUsers.end(); ++i)
							i->second->Send(value);
					}

				}
				
				BroadcastToSpectators(value);
			}
			void RoomUserManager::BroadcastToSerialUsers(Serializer::ISerializable& value, unsigned int ignoreUserSerial, const std::wstring& ignoreUserName)
			{
				AutoLock lock(cs);

				{
					for (serialUserMap::iterator i = serialUsers.begin(); i != serialUsers.end(); ++i) {
						if (ignoreUserSerial != i->first) {
							i->second->Send(value);
						}
					}
				}
				BroadcastToSpectators(value, ignoreUserName);
			}

			void RoomUserManager::BroadcastToAchivementUsers(Serializer::ISerializable& value)
			{
				AutoLock lock(cs);

				{
					for (serialUserMap::iterator i = serialUsers.begin(); i != serialUsers.end(); ++i) {
						;// i->second->achievement.Send(value);
					}
				}
			}


			void RoomUserManager::BroadcastToAllEnteredUsers(Serializer::ISerializable& value)
			{
				AutoLock lock(cs);

				for (stringUserMap::iterator i = nameUsers.begin(); i != nameUsers.end(); ++i)
					i->second->Send(value);
			}


			void RoomUserManager::BroadcastToTeam(int teamNo, Serializer::ISerializable& value)
			{
				AutoLock lock(cs);

				for (serialUserMap::iterator i = serialUsers.begin(); i != serialUsers.end(); ++i)
					if (i->second->GetTeamNumber() == teamNo)
						i->second->Send(value);
			}

			void RoomUserManager::BroadcastFishingInfo(bool fishing, Serializer::ISerializable& value)
			{
				AutoLock lock(cs);

				for (serialUserMap::iterator i = serialUsers.begin(); i != serialUsers.end(); ++i)
					if (fishing && i->second->IsFishing())
						i->second->Send(value);
					else
						i->second->Send(value);
			}

			void RoomUserManager::KickVoteBroadCasting(Serializer::ISerializable& value, std::wstring& targetName)
			{
				AutoLock lock(cs);

				for (serialUserMap::iterator i = serialUsers.begin(); i != serialUsers.end(); ++i)
				{
					if (i->second->GetName() == targetName)
						continue;

					i->second->Send(value);
				}
			}


			void RoomUserManager::Flush()
			{
				AutoLock lock(cs);

				for (stringUserMap::iterator i = nameUsers.begin(); i != nameUsers.end(); ++i)
					i->second->Flush();
			}


			void RoomUserManager::KickAllUsers()
			{
				AutoLock lock(cs);

				for (stringUserMap::iterator i = nameUsers.begin(); i != nameUsers.end(); ++i)
					i->second->Close();
			}


			void RoomUserManager::KickTeam(int teamNo)
			{
				AutoLock lock(cs);

				for (stringUserMap::iterator i = nameUsers.begin(); i != nameUsers.end(); ++i)
				{
					UserSharedPtr user = i->second;

					if (user == NULL)
						continue;

					if (user->GetTeamNumber() == teamNo)
						user->Close();
				}
			}


			void RoomUserManager::SetExpPanelty()
			{
				AutoLock lock(cs);

				for (stringUserMap::iterator i = nameUsers.begin(); i != nameUsers.end(); ++i)
				{
					UserSharedPtr user = i->second;

					if (user == NULL)
						continue;

					user->SetPenaltyExpFactor(true);
				}
			}


			void RoomUserManager::UpdateExpFactor(Logic::ILogic* logic)
			{
				AutoLock lock(cs);

				for (stringUserMap::iterator i = nameUsers.begin(); i != nameUsers.end(); ++i)
				{
					UserSharedPtr user = i->second;

					if (user == NULL)
						continue;

					Logic::Player* player = user->GetPlayer();

					if (player == NULL)
						continue;

					logic->SetPlayerExpFactor(user->GetPlayer(), user->GetExpFactor());
				}
			}


			void RoomUserManager::LoadStage(const StageLicense& targetStage)
			{
				AutoLock lock(cs);
				LoggerInstance().Info("RoomUserManager::LoadStage {},{},{}", targetStage.StageGroupHash,targetStage.Level,targetStage.Difficulty);
					for (serialUserMap::iterator i = serialUsers.begin(); i != serialUsers.end(); ++i)
						i->second->LoadStage(targetStage);
			}


			void RoomUserManager::StageLifeInit(const Database::Info::StageGroup* info, bool forceToInitialize)
			{
				AutoLock lock(cs);

				if (info->SharedLife && forceToInitialize == false)
				{
					for (serialUserMap::iterator i = serialUsers.begin(); i != serialUsers.end(); ++i)
					{
						Logic::Player* player = i->second->GetPlayer();
						if (player == NULL)
							continue;

						uint16 life = player->GetPlayerData().Life;
						i->second->SetPlayerInitLife(life);
					}
				}
				else
				{
					uint16 life = info->InitailLife;
					for (serialUserMap::iterator i = serialUsers.begin(); i != serialUsers.end(); ++i)
					{
						if ((info->GameType & Constants::GameTypes::CooperativeGameTypeMask) && i->second->GetCharacterStateFlags().IsPcRoom)
							i->second->SetPlayerInitLife(life + 1);
						else
							i->second->SetPlayerInitLife(life);
					}
				}
			}

			void RoomUserManager::CashItemView(UserSharedPtr user, std::pair< uint16, uint16 > flag)
			{
				if (flag.first == 0) {
					std::map< XRated::Serial, std::pair< uint16, uint16 > >::iterator iCashView = cashItemViewList.find(user->GetSerial());
					if (iCashView != cashItemViewList.end()) {
						cashItemViewList.erase(iCashView);
						LoggerInstance().Info("[RoomUserManager::CashItemView] User removed from cashitemview list. ({})", user->GetSerial());
					}

				}
				else {
					cashItemViewList[user->GetSerial()] = flag;
					LoggerInstance().Info("[RoomUserManager::CashItemView] User added to cashitemview list. ({})", user->GetSerial());
				}

				Protocol::FromServer::CashItemView packet;
				packet.userlist[user->GetSerial()] = flag;
				BroadcastToAllEnteredUsers(packet);
			}
			std::pair< uint16, uint16 > RoomUserManager::GetCashItemViewFlag(const UserSharedPtr user)
			{
				std::map< XRated::Serial, std::pair< uint16, uint16 > >::iterator iCashView = cashItemViewList.find(user->GetSerial());
				if (iCashView != cashItemViewList.end()) {
					return iCashView->second; //cashItemViewList[user->GetSerial()];
				}
				else
					return std::make_pair(0, 0);
			}

			int RoomUserManager::GetCashItemViewCnt() const
			{
				return cashItemViewList.size();
			}

			void RoomUserManager::ClearCashItemViewList()
			{
				cashItemViewList.clear();
			}

			void RoomUserManager::SendCashItemViewInfo(UserSharedPtr user)
			{
				Protocol::FromServer::CashItemView packet;
				packet.userlist = cashItemViewList;

				user->Send(packet);
			}

			bool RoomUserManager::IsExist(UserSharedPtr user)
			{
				AutoLock lock(cs);

				for (stringUserMap::iterator i = nameUsers.begin(); i != nameUsers.end(); ++i)
					if (i->second == user)
						return true;

				return false;
			}

			bool RoomUserManager::IsExist(const std::wstring& userName)
			{
				AutoLock lock(cs);

				for (stringUserMap::iterator i = nameUsers.begin(); i != nameUsers.end(); ++i)
					if (i->second->GetName() == userName)
						return true;


				return false;
			}


			int RoomUserManager::GetTeamCnt(int teamNo)
			{
				AutoLock lock(cs);

				int teamCnt = 0;
				for (serialUserMap::iterator i = serialUsers.begin(); i != serialUsers.end(); ++i)
					if (i->second->GetTeamNumber() == teamNo)
						++teamCnt;

				return teamCnt;
			}


			void RoomUserManager::MissionClear(bool success, Database::Info::StageGroup* stageGroup, uint16 accessLevel)
			{
				AutoLock lock(cs);
				volatile std::size_t serialUsers_size = serialUsers.size();  // to debug
				for (serialUserMap::iterator i = serialUsers.begin(); i != serialUsers.end(); ++i)
				{
					UserSharedPtr user = i->second;

					if (user->GetPlayer() == NULL) continue; // invalid user - actually, this happen.

					// Mission Clear when there is nextStage
					//if( success )
					//	user->AddStageLicense( nextStage );

					Protocol::FromServer::PvpLevelExp pvpLevelExp;
					pvpLevelExp.pvpLevel = user->GetPlayer()->GetPlayerData().PvpLevel;
					pvpLevelExp.pvpXp = user->GetPlayer()->GetPlayerData().PvpXp;
					user->Send(pvpLevelExp);

					if (user->GetExtraExpFactor() != 1.0f)
					{
						//Protocol::FromServer::Error extraExpMsg;
						Protocol::FromServer::Chat extraExpMsg;
						extraExpMsg.chattype = XRated::Constants::ChatTypes::SystemChat;
						extraExpMsg.message = L"[";
						extraExpMsg.message += user->GetName();
						extraExpMsg.message += L"]dasdsa[";
						extraExpMsg.message += user->GetExtraExpString();
						extraExpMsg.message += L"]dsaasd.";

						for (serialUserMap::iterator j = serialUsers.begin(); j != serialUsers.end(); ++j)
							j->second->Send(extraExpMsg);
					}

					if ((user->GetState() == User::STATE::TRADE) ||
						(user->GetState() == User::STATE::TRADEREADY))
					{
						//user->CancelTrade();
					}

					user->MissionClear(success, stageGroup, accessLevel);

					user->UpdateInfos();
				}
			}


			void RoomUserManager::ObjectDestroyed(uint32 id, uint8 team)
			{
				AutoLock lock(cs);

				for (serialUserMap::iterator i = serialUsers.begin(); i != serialUsers.end(); ++i)
				{
					i->second->ObjectDestroyed(id, team);
				}
			}


			void RoomUserManager::StageMoving()
			{
				AutoLock lock(cs);
				for (serialUserMap::iterator i = serialUsers.begin(); i != serialUsers.end(); ++i)
				{
					UserSharedPtr user = i->second;
					user->CheckExpiredItems();
					user->UpdateInfos();
					user->LockItems(true);
					user->SetPlayerStylePointState(StylePoint::State::Stop);
					user->SetUltimateSkillFlag(false);

					UserManagerInstance().StageChanged(user);
				}
			}

			void RoomUserManager::StageMoving(const std::vector<std::wstring>& userList)
			{
				AutoLock lock(cs);
				for (serialUserMap::iterator i = serialUsers.begin(); i != serialUsers.end(); ++i)
				{
					if (std::find(userList.begin(), userList.end(), i->second->GetName()) != userList.end()) {
						UserSharedPtr user = i->second;
						user->CheckExpiredItems();
						user->UpdateInfos();
						user->LockItems(true);
						user->SetPlayerStylePointState(StylePoint::State::Stop);

						UserManagerInstance().StageChanged(user);
					}
				}
			}


			void RoomUserManager::GetUserNames(std::vector<std::wstring>& userNames) // param is output
			{
				userNames.reserve(serialUsers.size());

				AutoLock lock(cs);
				for (serialUserMap::iterator i = serialUsers.begin(); i != serialUsers.end(); ++i)
					userNames.push_back(i->second->GetName());
			}


			void RoomUserManager::GetTeamNames(int teamNo, std::vector<std::wstring>& userNames) // param is output
			{
				AutoLock lock(cs);
				for (serialUserMap::iterator i = serialUsers.begin(); i != serialUsers.end(); ++i)
				{
					if (teamNo == i->second->GetTeamNumber())
						userNames.push_back(i->second->GetName());
				}
			}

			stringUserMap& RoomUserManager::GetNameUsers()
			{
				return nameUsers;
			}

			serialUserMap& RoomUserManager::GetSerialUsers()
			{
				return serialUsers;
			}

			int RoomUserManager::TotalCnt() const
			{
				return totalCnt;
			}

			int RoomUserManager::MaxCnt() const
			{
				return maxCnt;
			}

			int RoomUserManager::NowCnt() const
			{
				AutoLock lock(cs); return (int)nameUsers.size();
			}

			int	RoomUserManager::CountCurrent(bool includeSpectator) const
			{
				AutoLock lock(cs);

				if (includeSpectator)
					return nameUsers.size();

				int count = nameUsers.size();

				for (stringUserMap::const_iterator itr = nameUsers.begin(); itr != nameUsers.end(); ++itr)
				{
					if (itr->second->GetCharacterStateFlags().IsSpectator == true)
						--count;
				}

				return count < 0 ? 0 : count;
			}

			void RoomUserManager::CheckMailAlaram(UserSharedPtr user)
			{
				AutoLock lock(cs);
				AutoLock updateLock(updateCS);

				//Add to mail alarm list
				std::wstring tmpName = user->GetName();
				StringUtil::ToLowerCase(tmpName);
				mailAlarm.userlist[tmpName] = false;

				Net::Api request("Mail.Alarm");
				request << user->GetName();
				request.GetAsync(this, &RoomUserManager::MailAlarmed);
			}

			void RoomUserManager::MailAlarmed(const UserSharedPtr&, const Net::Answer& packet)
			{
				AutoLock lock(updateCS);

				if (packet) // ok
				{
					uint16 i = 0;
					for(auto& mail : packet.at("mails").get<json>()) {
						std::wstring tmpName = mail.at("characterName").get<std::wstring>();
						StringUtil::ToLowerCase(tmpName);
						std::map<std::wstring, bool>::iterator iAlarmUser = mailAlarm.userlist.find(tmpName);
						if (iAlarmUser != mailAlarm.userlist.end()) {
							if (iAlarmUser->second == false)
							{
								iAlarmUser->second = true;

								stringUserMap::iterator iUser = std::find_if(nameUsers.begin(), nameUsers.end(), UserFinder(mail.at("characterName").get<std::wstring>()));
								if (iUser != nameUsers.end()) {
									Protocol::FromServer::Mail::MailAlarmed response;
									iUser->second->Send(response);
									iUser->second->MailAlarmed();
								}
							}
						}
						++i;
					}
				}
				else {
					LoggerInstance().Error("Error occurred when retrieving mail alarm list. [{}]", packet.errorMessage );
				}
			}

			bool RoomUserManager::MailAlarm::Update(float dt)
			{
				if (INTERVAL <= 0)
					return false;

				if (elapsedTime > INTERVAL) {
					elapsedTime -= INTERVAL;
					return true;
				}
				elapsedTime += dt;
				return false;
			}

			void RoomUserManager::MailAlarm::Init()
			{
				userlist.clear();
				elapsedTime = (float)Math::Random(0, 30); 
			}

			RoomUserManager::UserFinder::UserFinder(const String& name) : userName(name) {
				StringUtil::ToLowerCase(userName);
			}

			bool RoomUserManager::UserFinder::operator()(std::map<const std::wstring, UserSharedPtr>::value_type& i)
			{
				String temp(i.first);
				StringUtil::ToLowerCase(temp);
				return temp == userName;
			}

}
	}
}
