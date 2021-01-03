#include "Room.h"
#include <StageServer/User/UserManager.h>
#include <StageServer/User/User.h>
#include <Core/ErrorDefinition.h>
#include <StageServer/Protocol/FromServer.h>
namespace Lunia {
	namespace XRated {
		namespace StageServer {
			const float fixedWaitRoomDeleteTime = 60.0f;

			Room::Room(const uint16& index)
			{
				m_Logic = Logic::CreateLogic();
				m_Logic->SetEventListener(*this);
				m_ThreadIndex = UINT16_MAX;

			}
			uint16 Room::UserCount() const
			{
				return m_UserManager.NowCount();
			}
			void Room::Clear()
			{
				AutoLock lock(m_Mtx);

				if (GetRoomKind() != Common::SQUARE)
				{
					Net::Api request("StageOver");
					request << m_RoomIndex;
					request.GetAsync();
					//Stage::CoinItemPurchaseManagerInstance().UndoPurchaseCoinItem(GetRoomID());
				}

				m_CurrentId++;

				m_Active = false;
				m_Loading = false;
				m_CountReviveLimit = 0;
				m_PartyDeathCount = 0;
				m_SkillUseCount = 0;
				m_UserManager.Clear();
				//m_partyManager.Clear();
				//m_Voting.Clear();
				//m_AutoVoting.Clear();
				//m_Team1Voting.Clear();
				//m_Team2Voting.Clear();
				//m_PersonalVoting.Clear();

				//m_ClearedTime = 0.0f;
				m_OpenRoom = 0;
				m_ThreadIndex = UINT16_MAX;
				m_WaitingUsers.clear();

				m_MissionCleared = false;
				m_NowCampfire = false;
				m_WaitRoomDelete = false;

				m_CurrentFloor = 0;
				//m_StartingUserCount = 0;
				m_FloorClearedTime = 0;

				m_DieCountSerial.clear();
				m_KillCountSerial.clear();
			}
			bool Room::Update(const float& dt)
			{
				if(m_WaitRoomDelete){
					m_WaitRoomDeleteTime -= dt;
					if (m_WaitRoomDeleteTime < 0.0f) {
						LoggerInstance().Info("Room({0})::Update - Clear() no one in the room", m_RoomIndex);
						Clear();
						return false;
					}
				}
				{
					AutoLock lock(m_Mtx);
					if (!m_Active)
						return true;
					if (!m_Loading) {
						if (m_LastThreadIndex != m_ThreadIndex)
						{
							Logger::GetInstance().Info(L"roomCs(index:{0}) updating thread is changed({1} to {2})", m_RoomIndex, m_LastThreadIndex, m_ThreadIndex);
							m_LastThreadIndex = m_ThreadIndex;
						}
						assert(m_ThreadIndex != UINT16_MAX);
						m_Logic->Update(dt);
					}
				}
				m_UserManager.Update(dt);

				CheckLoadingTime(dt);

				m_UserManager.Flush();

				if (m_WaitRoomDelete)
				{
					m_WaitRoomDeleteTime -= dt;
					if (m_WaitRoomDeleteTime <= 0.0f)
					{
						Clear();
						LoggerInstance().Info("Room({0}) Released by inactivity", m_RoomIndex);

						return	false;
					}
				}

				return true;
			}
			void Room::SetThreadIndex(const uint16& i)
			{
				AutoLock lock(m_Mtx);
				m_Active = false; // kind of hack - because when SetThreadIndex is set, logic should not be updated before initialize 
				m_ThreadIndex = i;
			}
			void Room::CheckLoadingTime(const float& dt)
			{
				if (GetRoomKind() == Common::ROOMKIND::PVP && pvp.SyncEnteredUserCnt == pvp.SyncStartUserCnt) {
					m_LoadingTime += dt;
					if (m_LoadingTime > 180.0f)
						; // kick all users using user manager
				}
			}
			Constants::GameTypes Room::GetRoomType() const
			{
				return m_StageGroupInfo->GameType;
			}
			Common::ROOMKIND Room::GetRoomKind() const
			{
				if (GetRoomType() & Constants::GameTypes::CooperativeGameTypeMask)
				{
					return Common::STAGE;
				}
				else if (GetRoomType() & Constants::GameTypes::PvpGameTypeMask)
				{
					return Common::PVP;
				}
				else //if(gametype& AllM::XRated::Constants::GameTypes::PeacefulGameTypeMask) 
				{
					return Common::SQUARE;
				}
			}
			const Logic::ILogic::UpdateInfo& Room::GetLogicUpdateInfo()
			{
				return m_Logic->GetUpdateInfo();
			}
			uint16 Room::GetIndex()
			{
				return m_RoomIndex;
			}
			uint16 Room::GetThreadIndex()
			{
				return m_ThreadIndex;
			}
			bool Room::JoinUser(UserSharedPtr user, const std::string& roomPass) //roomPass is just being used a logger?
			{
				AutoLock lock(user->GetSyncObject());
				if (m_NowCampfire) {
					Protocol::FromServer::Error sendPacket;
					sendPacket.errorcode = Errors::UnableToJoinStage;
					user->Send(sendPacket);
					Logger::GetInstance().Error("Room({0})::JoinUser() - Could not join user={0} to the room={1}", m_RoomIndex, user->GetId(), this->GetIndex());
					return false;
				}
				if (!SetStage(user->GetCurrentStage(), roomPass, user->GetActivationSerial(), user->GetCharacterName())) {
					LoggerInstance().Error("Room({0})::JoinUser() - SetStage = false", m_RoomIndex);
					Protocol::FromServer::Error error;
					error.errorcode = Errors::UnableToJoinStage;
					user->Send(error);
					return false;
				}
				if (!(m_StageGroupInfo->GameType & Constants::GameTypes::PvpGameTypeMask)) {
					if (user->GetCharacterStateFlags().IsSpectator || user->GetTeamNumber() == 255) {
						LoggerInstance().Error("Room({0})::JoinUser() - User:{0} Joined Wrong Game - try to join stage, but joined pvp, t {1}, {2}, {3}", m_RoomIndex, user->GetSerial(), user->GetTeamNumber(), user->GetCharacterStateFlags().IsSpectator ? "spectator" : "normal");
						user->Close();
						return false;
					}
				}
				if (((m_CurrentStage.StageGroupHash != user->GetCurrentStage().StageGroupHash) ||
					(m_CurrentStage.Level != user->GetCurrentStage().Level)) /*&& isEnablePartyJoin == false*/) {
					LoggerInstance().Error("Room({0})::JoinUser() - currentStage({0}/{1}) != userStage({2}/{3})", m_RoomIndex, m_CurrentStage.StageGroupHash, m_CurrentStage.Level, user->GetCurrentStage().StageGroupHash, user->GetCurrentStage().Level);
					Protocol::FromServer::Error error;
					error.errorcode = Errors::InvalidRoomNumber;
					user->Send(error);
					return false;
				}
				if (!user->IsAbleToJoinStage(m_CurrentStage) /*&& isEnablePartyJoin == false*/) {
					LoggerInstance().Error(L"Room({0})::JoinUser() - Not EnableJoinStage && Not EnablePartyJoin", m_RoomIndex);
					Protocol::FromServer::Error error;
					error.errorcode = Errors::UnableToJoinStage;
					user->Send(error);
					return false;
				}
				if (!SideStageJoinCheck()) {
					LoggerInstance().Error("Room({0})::JoinUser() - SideStageJoinCheck() == false", m_RoomIndex);
					Protocol::FromServer::Error error;
					error.errorcode = Errors::UnableToJoinStage;
					user->Send(error);
					return false;
				}
				if (!CapacityCheck(user)) {
					LoggerInstance().Error("Room({0})::JoinUser() - CapacityCheck() == false", m_RoomIndex);
					return false;
				}
				//user->RoomJoined(shared_from_this(), m_CurrentStage);
				return false;
			}
			bool Room::PartUser(UserSharedPtr user)
			{
				if (user->GetPlayer() != NULL) {
					m_Logic->Part(user->GetPlayer());
					m_UserManager.DelSerialUser(user->GetSerial());
				}
				/*
				if (user->GetPartyIndex() >= 0)
					PartyLeave(user);

				if (user->IsFishing())
					RemoveFishingUser(user->GetSerial());
				*/
				user->RoomParted();
				if (m_UserManager.NowCount() == 0) {
					m_WaitRoomDelete = true;
					m_WaitRoomDeleteTime = fixedWaitRoomDeleteTime;
				}
				if (GetRoomType() & Constants::GameTypes::BattleGroundType) {
					LoggerInstance().Warn("Room::PartUser BattleGroundType missing implementation");
				}
				switch (GetRoomKind()) {
				case Common::ROOMKIND::SQUARE:
					if (m_UserManager.NowCount() == 0)
						m_Locked = false; // if the room was private it was "locked" previously?
					break;
				case Common::ROOMKIND::PVP:
					if (!pvp.GameStarted && user->GetCharacterStateFlags().IsSpectator == true)
						return false;//do nothing.
					if (pvp.SyncEnteredUserCnt < pvp.SyncStartUserCnt && user->GetCharacterStateFlags().IsSpectator == false) {
						Protocol::FromServer::Error error;
						error.errorcode = Errors::Unexpected;
						error.errorstring = fmt::format(L"{0}- failed to join", user->GetSerial()).c_str();
						m_UserManager.BroadcastToAllEnteredUsers(error);
						m_UserManager.KickAllUsers();
					}
				}
				//slimeRaceGame.PlayerLeaved( user->GetName() );
				LoggerInstance().Info("Room(1)::PartUser() - user={1} left", m_RoomIndex, user->GetSerial());
				return true;
			}
			void Room::UpdateExpFactor(){
				m_UserManager.UpdateExpFactor(m_Logic);
			}
			void Room::NoticeHolidayEvent(const uint32& eventId, bool start)
			{
				m_Logic->NoticeHolidayEvent(eventId, start);
			}
			void Room::Initialized(Database::Info::StageInfo* /*info*/, uint16 uniqueId)
			{
				AutoLock lock(m_Mtx);
				m_Guilds.clear();
				m_Guilds.reserve(10);
				if (m_CurrentId != uniqueId) {
					LoggerInstance().Exception("Room({0})::Initialized - id doesn't match together {1}!={2}", m_RoomIndex, m_CurrentId, uniqueId);
					return;
				}
				if (!m_Loading) {
					LoggerInstance().Exception("Room({0})::Initialized - Loading isn't requested. Initializing logic canceled in some reason.", m_RoomIndex);
					return;
				}
				m_Active = true;
				m_Loading = true;

				LoggerInstance().Info(L"initialized room(index:{0}, threadIndex:{1}) info {2}"\
					L", waiting:{3}, nowcnt:{4}, maxcnt:{5}, capacity:{6}, active:{7}, loading:{8}, logicloading:{9}"
					, m_RoomIndex, m_ThreadIndex, (m_StageInfo ? m_StageInfo->Id : fmt::format(L"(null:{0},{1})", m_CurrentStage.StageGroupHash, m_CurrentStage.Level))
					, m_WaitingUsers.size(), m_UserManager.NowCount(), m_UserManager.MaxCount(), GetStageCapcacity(), m_Active, m_Loading, m_Logic->IsLoading()
				);
				while (!m_WaitingUsers.empty()) {
					auto& user = m_WaitingUsers.front();
					if (user != m_UserManager.GetUser(user->GetSerial()))
					{
						m_WaitingUsers.pop_front();
						continue;
					}

					LoggerInstance().Info(L"logic->Join() waitingUser({0})", user->GetSerial());
					m_Logic->Join(user->GetInitialPlayerData(), std::static_pointer_cast<void>(user));
					m_WaitingUsers.pop_front();
				}
			}
			std::mutex& Room::GetSyncRoom()
			{
				return m_Mtx;
			}
			void Room::NonPlayerCreated(NonPlayerData& data)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			bool Room::Tamed(Logic::Player* player, Serial familiar, Constants::Familiar::Type type)
			{
				LoggerInstance().Exception("Missing implementation");
				return false;
			}
			void Room::StructureCreated(CharacterData& data)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::VehicleCreated(CharacterData& data)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::ProjectileCreated(ObjectData& obj, const float3& targetPos, Serial target, const std::wstring& owner)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::ItemCreated(ObjectData& data, const std::wstring& owner, float dt, uint16 stackCount, bool isPrivateItem)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::ObjectCreated(ObjectData& data)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::ObjectDestroyed(Serial gameObjectSerial, Constants::ObjectType type, uint32 hash, bool silent, uint8 team, NonPlayerData::NpcType npcType)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::Died(Logic::Player* player)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::Revived(Logic::Player* player, uint16 totalReviveCount)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::InstantCoinRevived(Logic::Player* player, Logic::ILogic::InstantCoinReviveError error, uint64 orderNumber)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			bool Room::IsPvPRoom()
			{
				LoggerInstance().Exception("Missing implementation");
				return false;
			}
			void Room::AnimationChanged(Serial gameObjectSerial, uint32 animation, const float3& position, const float3& direction, const float param)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::CollisionStateChanged(Serial gameObjectSerial, bool collision, const float3& position)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::XpGained(Logic::Player* player, XRated::Constants::ExpAcquiredType type, uint64 storyXp, int32 pvpXp, int32 warXp, Serial beKilledNpc)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::GoldGained(Logic::Player* player, XRated::Constants::GoldRewardAcquiredType type, uint32 money)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			bool Room::AcquireItem(Logic::Player* player, uint32 itemId, Serial itemSerial, uint16 stackCount, InstanceEx instance)
			{
				LoggerInstance().Exception("Missing implementation");
				return false;
			}
			void Room::SendPvPRewardItemInfo(Database::Info::PvPRewardInfo rewardInfo)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::AcquireItemWithMailSupport(Logic::Player* player, uint32 itemId, uint16 stackCount)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			bool Room::PlayerCreated(Logic::Player* player)
			{
				LoggerInstance().Exception("Missing implementation");
				return false;
			}
			void Room::JoinFailed(std::shared_ptr<void> user)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::ItemUsed(uint32 returnValue, Logic::Player* player, const Database::Info::ItemInfo* info, int bag, int pos)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::Casted(Serial gameObjectSerial, uint32 skillGroupName, uint8 skilllevel)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::ItemEquipped(bool bequipped, Logic::Player* player, uint32 itemId, Database::Enchant::EnchantBitfields fields, uint32 itemOldId)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::EquipementSwapped(bool bequipped, Logic::Player* player, const std::vector<EquippedItem>& newEquipments)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::CashEquipementSwapped(bool bequipped, Logic::Player* player, const std::vector<EquippedItem>& newEquipments)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::ItemEquipToEquipChanged(bool bChanged, Logic::Player* player, Constants::Equipment from, Constants::Equipment to)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::SkillPointChanged(Logic::Player* player, uint32 skillGroupHash, int level)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::LevelUp(Logic::Player* player, XRated::Constants::ExpType expType, uint16 level, uint32 storedLevel)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::SkillPointGot(Logic::Player* player, int skillPoint)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::StateChanged(Serial gameObjectSerial, uint32 id, int level, Serial byWhom)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::StatusChanged(Serial gameObjectSerial, float3 pos, float3 dir, float hp, float mp, Serial byWhom, uint32 byWhat, uint32 sFlag, uint32 airComboCount)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			bool Room::AddItem(Logic::Player* player, uint32 id, Serial gameObjectSerial, int cnt)
			{
				LoggerInstance().Exception("Missing implementation");
				return false;
			}
			bool Room::AddItem(Logic::Player* player, Serial gameObjectSerial, int* arrayId, int* arrayCnt)
			{
				LoggerInstance().Exception("Missing implementation");
				return false;
			}
			bool Room::RemoveItem(Logic::Player* player, uint32 id, int cnt)
			{
				LoggerInstance().Exception("Missing implementation");
				return false;
			}
			bool Room::RemoveItem(Logic::Player* player, int* arrayId, int* arrayCnt)
			{
				LoggerInstance().Exception("Missing implementation");
				return false;
			}
			bool Room::ExamineItemSlotFromPlayer(Logic::Player* player, Database::Info::ItemInfo* info, int cnt)
			{
				LoggerInstance().Exception("Missing implementation");
				return false;
			}
			void Room::AcquirePvpItem(Logic::Player* player, uint32 itemHash)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::UsePvpItem(Logic::Player* player, uint32 itemHash)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::DropPvpItem(Logic::Player* player)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			uint8 Room::GetQuestState(Logic::Player* player, uint32 questHash)
			{
				LoggerInstance().Exception("Missing implementation");
				return uint8();
			}
			uint8 Room::SetQuestState(Logic::Player* player, uint32 questHash, uint8 newState)
			{
				LoggerInstance().Exception("Missing implementation");
				return uint8();
			}
			uint32 Room::GetQuestParameter(Logic::Player* player, uint32 questHash, uint8 paramIndex)
			{
				LoggerInstance().Exception("Missing implementation");
				return uint32();
			}
			uint32 Room::SetQuestParameter(Logic::Player* player, uint32 questHash, uint8 paramIndex, uint32 newValue)
			{
				LoggerInstance().Exception("Missing implementation");
				return uint32();
			}
			uint32 Room::GetItemCount(Logic::Player* player, const Database::Info::ItemInfo::IFindCondition& condition) const
			{
				LoggerInstance().Exception("Missing implementation");
				return uint32();
			}
			void Room::ChangePlayerStatusLimit(Logic::Player* player, const std::vector<uint32>& limits)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::ChangePlayerStatusRate(Logic::Player* player, const std::vector<float>& rate)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::LocPlayers(bool lock)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::SetFloor(int floor)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::CheckStateBundleForAchievement(uint32 stateBundleHash)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::DisplayTextEvent(Constants::DisplayTextType type, uint16 textId, float param)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::NoticeTextEvent(Logic::Player* player, Constants::DisplayTextType type, uint16 textId, float param)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::DisplayText(Constants::DisplayTextType type, const std::wstring& message)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::NoticeText(Logic::Player* player, Constants::DisplayTextType type, const std::wstring& message)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::DisplayTimer(Constants::DisplayTimerType timer, float seconds)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::MissionClear(bool success, uint8 team, const CriticalStatistics::StageStatistics& SS, uint32 dropCount)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::BroadcastEvent(int eventId, const std::wstring& param)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::NoticeEvent(Logic::Player* player, int eventId, const std::wstring& param)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::NoticeClientStageEvent(Logic::Player* player, int eventId, int eventParam)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::TeamChange(Serial object, int team)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::AddMinimapPing(int pingId, const float3 position, Constants::MiniMapPingType type)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::NoticeAddMinimapPing(int pingId, const float3 position, Constants::MiniMapPingType type, Logic::Player* player)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::RemoveMinimapPing(int pingId)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::PlayerDie(Serial objectSerial, Serial fromSerial)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::PlayerSave(Serial objectSerial, Serial fromSerial)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::StyplePointType(Serial objectSerial, XRated::Constants::StylePointType type)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::ContinueCountDown()
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::Paused(bool bPaused)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::GiveLicense(Logic::Player* player, uint32 licenseGroup, uint32 accessLevel)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::GiveCharacterLicense(Logic::Player* player, uint32 characterId)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::SectorChanged(Serial serial, Logic::Sector* sector, Constants::Direction direction)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			const PvpRoomInfo::BattleGroundInfoType& Room::GetBattleGroundInfo() const
			{
				return m_BattleGroundInfo;
			}
			int Room::GetKillCount(uint32 serial)
			{
				LoggerInstance().Exception("Missing implementation");
				return 0;
			}
			int Room::GetDeathCount(uint32 serial)
			{
				LoggerInstance().Exception("Missing implementation");
				return 0;
			}
			bool Room::Rebirthed(Logic::Player& player)
			{
				LoggerInstance().Exception("Missing implementation");
				return false;
			}
			void Room::RebirthRollBacked(Logic::Player& player)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::NoticeStylePointStateToAllUser(XRated::StylePoint::State state, uint8 team)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::GambleGameEnd(const std::vector<uint8>& rankList)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::GambleNextGame(Logic::Player* player, float seconds)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::GambleNextGame(float seconds)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			const StageLicense& Room::GetCurrentStageLicense()
			{
				return m_CurrentStage;
			}
			int Room::GetSlimeRaceIntervalInMin() const
			{
				LoggerInstance().Exception("Missing implementation");
				return 0;
			}
			bool Room::IsHolidayEventTime(uint32 eventId)
			{
				LoggerInstance().Exception("Missing implementation");
				return false;
			}
			void Room::PetInfoUpdated(Logic::Player* player, const XRated::Pet& pet, const XRated::Serial petSerial, const std::wstring& petExpression)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::PetCaredBySchool(Logic::Player* player, const XRated::PetCaredBySchool& caredPet, const XRated::Pet& pet)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::PetTakenOutFromSchool(Logic::Player* player, const XRated::PetCaredBySchool& caredPet, const XRated::Pet& pet)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::PetRevived(Logic::Player* player)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::PetChange(Logic::Player* player, uint32 petItemHash, XRated::GlobalSerial petItemSerial)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::PetDeleted(Logic::Player* player, XRated::GlobalSerial petSerial)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::PetItemEquipped(bool bequipped, XRated::GlobalSerial petSerial, XRated::Serial petObjectSerial, Logic::Player* player, uint32 itemId, Database::Enchant::EnchantBitfields fields, uint32 itemOldId)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::AddedSkillPointPlus(Logic::Player* player, uint16 currentSkillPoint, uint16 skillPointPlus, uint16 storedSkillPoint)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::AddedGuildUser(Serial player)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::RemovedGuildUser(Serial player)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::UpdateGuildExpFactor(int guildId)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::ChangeWeatherToNone(const float fadeIntime)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::ChangeWeatherToClear(const float fadeIntime)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::ChangeWeatherToRain(const float fadeIntime)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::ChangeWeatherToSnow(const float fadeIntime)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::ChangeWeatherToAqua(const float fadeIntime)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			//PRIVATE METHODS///////////////////////////////////////////////////////////////////////////////
			bool Room::SetStage(StageLicense& targetStage, const std::string& roomPass, const int64& pActivationSerial, const std::wstring& userName)
			{
				AutoLock lock(m_Mtx);
				if (m_UserManager.NowCount() != 0)
					return true;
				if (m_Logic->IsLoading()) {
					LoggerInstance().Error("Room::SetStage Failed - Already loading");
					return false;
				}
				m_CurrentId++;
				/*
					We might be able to remove these verifications hence we already did this when the user Authed. But, we shall see..
				*/
				m_StageGroupInfo = Database::DatabaseInstance().InfoCollections.StageGroups.Retrieve(targetStage.StageGroupHash);
				if (m_StageGroupInfo == NULL) {
					LoggerInstance().Warn(L"Room({0})::SetStage - m_StageGroupInfo(NULL) GroupHash:{1},UserName:{2}", m_RoomIndex, targetStage.StageGroupHash, userName);
					return false;
				}

				m_StageInfo = Database::DatabaseInstance().InfoCollections.Stages.Retrieve(m_StageGroupInfo->StageHashes[targetStage.Level]);
				if (m_StageInfo == NULL) {
					LoggerInstance().Warn(L"Room({0})::SetStage - m_StageInfo(NULL) GroupHash:{1},UserName:{2},TargetLevel:{3}", m_RoomIndex, targetStage.StageGroupHash, userName, targetStage.Level);
					return false;
				}

				if (this->m_ActivationSerial == pActivationSerial && !(m_StageGroupInfo->GameType & Constants::PeacefulGameTypeMask)) {
					LoggerInstance().Warn("Room({0})::SetStage - pActivationSerial:{1} invalid.", m_RoomIndex, pActivationSerial);
					return false;
				}
				m_CurrentStage = targetStage;
				m_CurrentFloor = 0;
				m_StartingUserCount = 0;
				m_FloorClearedTime = 0;

				LoggerInstance().Info("Room({0})::SetStage - Stage:{1}-{2}-{3}, Members:{4},pActiveSerial:{5},Current:{6}",
					m_RoomIndex,
					m_CurrentStage.StageGroupHash,
					m_CurrentStage.Level,
					m_CurrentStage.Difficulty,
					UserCount(),
					pActivationSerial,
					m_ActivationSerial);

				m_LoadingTime = 0.0f;

				m_PartyChannelName = L"";
				if (GetRoomType() & Constants::CooperativeGameTypeMask)
					m_PartyChannelName = L"#" + userName + L"<CS>" + DateTime::Now().ToString(DateTime::Format::Unseparated);
				else if (GetRoomType() & Constants::PvpGameTypeMask) {
					pvp.SyncStartUserCnt = m_StartingUserCount;
					pvp.SyncEnteredUserCnt = 0;
					pvp.GameStarted = false;
					pvp.BgmStarted = false;
					pvp.bSendStartEvent = false;
				}

				m_ActivationSerial = pActivationSerial;

				m_Loading = true;

				m_OpenRoom = roomPass.empty();

				m_Logic->Init(m_CurrentStage.StageGroupHash, m_CurrentStage.Level, m_CurrentId);

				m_MissionCleared = false;
				m_NowCampfire = false;
				m_Locked = false;

				if (GetRoomKind() == Common::PVP) {
					m_DieCountSerial.clear();
					m_KillCountSerial.clear();
				}
				return true;
			}
			uint16 Room::GetStageCapcacity() const
			{
				return m_StageGroupInfo->Capacity;
			}
			bool Room::SideStageJoinCheck() const
			{
				if (GetRoomKind() != Common::STAGE)
					return true;

				/* if the stage shares player lives, the stage group must be started from first stage */
				if (m_StageGroupInfo->SharedLife && m_CurrentStage.Level != 0)
					return false;

				return true;
			}
			bool Room::CapacityCheck(UserSharedPtr user) const
			{
				if (user->GetCharacterStateFlags().IsSpectator) return true; // unlimited spectator limit without no policy

				switch (GetRoomKind())
				{
				case Common::STAGE:
					if (m_Logic->IsLoading())
					{
						if (m_UserManager.NowCount() >= user->GetRoomCapacity())
						{
							LoggerInstance().Error(L"Room({0})::CapacityCheck() - invalid number(%d/%d) of users in stage(%s)", m_RoomIndex, m_UserManager.NowCount(), user->GetRoomCapacity(), m_StageInfo ? m_StageInfo->Id.c_str() : L"blank");
							Protocol::FromServer::Error error;
							error.errorcode = Errors::StageFulled;
							user->Send(error);
							return false;
						}
					}
					else if (m_UserManager.NowCount() >= GetStageCapcacity())
					{
						LoggerInstance().Info("Room({0})::CapacityCheck() - Error: [STAGE] m_UserManager.NowCount({1}) >= GetStageCapcacity({2})", m_RoomIndex, m_UserManager.NowCount(), GetStageCapcacity());
						Protocol::FromServer::Error error;
						error.errorcode = Errors::StageFulled;
						user->Send(error);
						return false;
					}
					break;
				case Common::PVP:
					if (GetRoomType() & XRated::Constants::GameTypes::BattleGroundType)
					{

					}
					else if (m_UserManager.NowCount() >= user->GetRoomCapacity())
					{
						LoggerInstance().Error("Room({0})::CapacityCheck() - Error: [PVP] m_UserManager.NowCount({1}) >= user->GetRoomCapacity({2})", m_RoomIndex, m_UserManager.NowCount(), user->GetRoomCapacity());
						Protocol::FromServer::Error error;
						error.errorcode = Errors::StageFulled;
						user->Send(error);
						return false;
					}
					break;

				case Common::SQUARE:
				{
					const std::vector<SquareStruct> squareInfos = ConfigInstance().Get<std::vector<SquareStruct>>("SquareList");
					uint16 capacity = 80;
					if (m_RoomIndex >= 0 && m_RoomIndex < squareInfos.size())
						capacity = squareInfos[m_RoomIndex].Capacity;

					if (m_UserManager.NowCount() >= capacity)
					{
						LoggerInstance().Info("Room({0})::CapacityCheck() - Error: [SQUARE] m_UserManager.NowCnt({1}) >= {2}", m_RoomIndex, m_UserManager.NowCount(), capacity);
						Protocol::FromServer::Error error;
						error.errorcode = Errors::ServerIsFull;
						user->Send(error);
						return false;
					}
				}
				break;
				default:
					return false;
				}

				return true;
			}
		}
	}
}