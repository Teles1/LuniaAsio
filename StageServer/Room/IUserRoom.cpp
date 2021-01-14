#include <StageServer/Room/Room.h>
#include <Core/ErrorDefinition.h>
#include <StageServer/User/UserManager.h>
#include <StageServer/User/User.h>
namespace Lunia {
	namespace XRated {
		namespace StageServer {
			//IUserRoom Implementation
			void Room::DebugCommand(UserSharedPtr user, const String& msg)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::SpectatorChat(const String& characterName, Protocol::ToServer::Chat& chat)
			{
				Protocol::FromServer::SpectatorChat sendchat;
				sendchat.characterName = characterName;
				sendchat.chattype = chat.chattype;
				sendchat.message = chat.message;

				switch (chat.chattype)
				{
				case XRated::Constants::ChatTypes::TeamChat:
				{
					m_UserManager.BroadcastToSpectators(sendchat);
				}
				break;
				}
			}
			void Room::Chat(const uint64& serial, Protocol::ToServer::Chat& chat)
			{
				UserSharedPtr user = m_UserManager.GetUser(serial);
				if (!user)
					return;

				Protocol::FromServer::Chat sendchat;
				sendchat.playerserial = serial;
				sendchat.chattype = chat.chattype;
				sendchat.message = chat.message;

				switch (chat.chattype)
				{
				case XRated::Constants::ChatTypes::TeamChat:
				{
					m_UserManager.BroadcastToTeam(user->GetTeamNumber(), sendchat);
				}
				break;

				default:
					m_UserManager.BroadcastToSerialUsers(sendchat);
					break;
				}
			}
			void Room::Voice(const uint64& serial, Protocol::ToServer::Voice& voice)
			{
				Protocol::FromServer::Voice sendvoice;
				sendvoice.playerserial = serial;
				sendvoice.chattype = voice.chattype;
				sendvoice.messageid = voice.messageid;

				auto user= m_UserManager.GetUser(serial);

				switch (GetRoomKind())
				{
				case Common::STAGE:
					m_UserManager.BroadcastToSerialUsers(sendvoice);
					break;
				case Common::SQUARE:
					m_UserManager.BroadcastToSerialUsers(sendvoice); // TELES TEMPORARY
					//partyMgr.BroadCasting(user->GetPartyIndex(), sendvoice);
					break;
				case Common::PVP:
					m_UserManager.BroadcastToTeam(user->GetTeamNumber(), sendvoice);
					break;
				}
			}
			void Room::PartyInvite(UserSharedPtr user, const uint64& serial)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::PartyAccept(UserSharedPtr accepter, UserSharedPtr inviter)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::PartyLeave(UserSharedPtr user)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::NotifySpectatorLeft(UserSharedPtr user)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			bool Room::Command(XRated::Logic::Player* player, XRated::Constants::Command command, XRated::Constants::Direction dir)
			{
				if (GetRoomKind() == Common::SQUARE)
				{
					switch (command)
					{
					case Constants::Command::Move:
					case Constants::Command::Dash:
					case Constants::Command::Stop:
						break;
					default:
						return false; // unable to use other commands
					}
				}

				m_Logic->Command(player, command, dir);
				return true;
			}
			void Room::Cast(XRated::Logic::Player* player, uint32 skill)
			{
				if (!player) {
					LoggerInstance().Error("Room::Cast() - player == NULL");
					return;
				}
				PlayerData& data = player->GetPlayerData();
				UserSharedPtr user = std::static_pointer_cast<User>(data.user);
				uint32 classHash = XRated::Constants::GetClassHash(user->GetClassType());

				Database::Info::SkillInfoList::SkillGroup* skillGroup = Database::DatabaseInstance().InfoCollections.Skills.GetSkillGroupInfo(classHash, skill);
				if (!skillGroup)
				{
					LoggerInstance().Error("Room::Cast() - SkillGroup == NULL");
					return;
				}

				if (skillGroup->SKillType == Database::Info::SkillInfoList::SkillGroup::Type::Produce)
				{
					if (!user->IsEnoughSlotCountByProduceSkill(skill))
					{
						Protocol::FromServer::Cast cast;
						cast.result = Protocol::FromServer::Cast::Result::NotEnoughSlot;
						cast.playerserial = player->GetPlayerData().BaseCharacter.BaseObject.GameObjectSerial;
						cast.skillgroupid = 0;
						cast.skilllevel = 0;
						user->Send(cast);
						return;
					}
				}
				else
				{
					if (GetRoomKind() == Common::SQUARE)
						return;

					if (m_StageInfo)
					{
						if (m_StageInfo->isSkillAvailable == false)
						{
							return;
						}
					}
				}

				m_Logic->Cast(player, skill);
			}
			void Room::LogicSkillPointUp(XRated::Logic::Player* player, Protocol::ToServer::SetSkillLevel& setskilllevel)
			{
				if (!player) {
					LoggerInstance().Error("Room::LogicSkillPointUp() - player == NULL");
					return;
				}
				if (setskilllevel.delta > 0)
					m_Logic->IncreaseSkillPoint(player, setskilllevel.skillgroupid);
				else
					m_Logic->DecreaseSkillPoint(player, setskilllevel.skillgroupid);
			}
			void Room::AddSkillPointPlus(XRated::Logic::Player* player, uint16 skillPointPlus)
			{
				if (!player) {
					LoggerInstance().Error("Room::AddSkillPointPlus() - player == NULL");
					return;
				}
				m_Logic->AddSkillPointPlus(player, skillPointPlus);
			}
			void Room::Revive(XRated::Logic::Player* player, bool self)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::DailyRevive(XRated::Logic::Player* player)
			{
				if (!player) {
					LoggerInstance().Error("Room::DailyRevive() - player == NULL");
					return;
				}
				m_Logic->DailyRevive(player);
			}
			void Room::InstantCoinRevive(XRated::Logic::Player* player, uint64 orderNumber)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::CreatePet(XRated::Logic::Player* player, const XRated::Pet& pet)
			{
				if (!player) {
					LoggerInstance().Error("Room::CreatePet() - player == NULL");
					return;
				}
				m_Logic->CreatePet(player, pet);
			}
			void Room::GivePresentToPet(XRated::Logic::Player* player, XRated::GlobalSerial petSerial, uint32 sellPrice, uint32 count)
			{
				if (!player) {
					LoggerInstance().Error("Room::GivePresentToPet() - player == NULL");
					return;
				}
				m_Logic->GivePresentToPet(player, petSerial, sellPrice, count);
			}
			void Room::addminimapping(const uint64& serial, float3 position)
			{
				Protocol::FromServer::AddMinimapPing minimapping;
				minimapping.pingid = serial;
				minimapping.position = position;
				minimapping.type = Constants::MiniMapPingType::PingPlayerEvent;

				UserSharedPtr user = m_UserManager.GetUser(serial);

				switch (GetRoomKind())
				{
				case Common::STAGE:
					m_UserManager.BroadcastToSerialUsers(minimapping);
					break;
				case Common::SQUARE:
					m_UserManager.BroadcastToSerialUsers(minimapping); // TELES
					//m_PartyManager.BroadCasting(user->GetPartyIndex(), minimapping);
					break;
				case Common::PVP:
					m_UserManager.BroadcastToTeam(user->GetTeamNumber(), minimapping);
					break;
				}
			}
			bool Room::Equip(XRated::Logic::Player* player, const Database::Info::ItemInfo* info, Database::Enchant::EnchantBitfields instance, XRated::Constants::Equipment pos)
			{
				if (!player) {
					LoggerInstance().Error("Room::Equip() - player == NULL");
					return false;
				}
				return m_Logic->Equip(player, info, instance, pos);
			}
			bool Room::EquipToEquip(Logic::Player* player, XRated::Constants::Equipment from, XRated::Constants::Equipment to)
			{
				if (!player) {
					LoggerInstance().Error("Room::EquipToEquip() - player == NULL");
					return false;
				}
				return m_Logic->ChangeEquipToEquip(player, from, to);
			}
			bool Room::SwapEquip(Logic::Player* player, const std::vector<EquippedItem>& newEquipments)
			{
				if (!player) {
					LoggerInstance().Error("Room::SwapEquip() - player == NULL");
					return false;
				}
				return m_Logic->SwapEquip(player, newEquipments);
			}
			bool Room::SwapCashEquip(Logic::Player* player, const std::vector<EquippedItem>& newEquipments)
			{
				if (!player) {
					LoggerInstance().Error("Room::SwapCashEquip() - player == NULL");
					return false;
				}
				return m_Logic->SwapCashEquip(player, newEquipments);
			}
			void Room::ValidateEquippedItems(Logic::Player* player)
			{
				if (!player)
				{
					LoggerInstance().Error("Room::ValidateEquippedItems() - Error : player == NULL");
					return;
				}

				m_Logic->ValidateEquippedItems(player);
			}
			bool Room::Use(XRated::Logic::Player* player, uint32 id, int bag, int pos)
			{
				if (!player) {
					LoggerInstance().Error("Room::Use() - player == NULL");
					return false;
				}
				Database::Info::ItemInfo* info = Database::DatabaseInstance().InfoCollections.Items.Retrieve(id);
				if (!info)
				{
					LoggerInstance().Error("Room::Use() - info == NULL");
					return false;
				}
				return m_Logic->Use(player, info, bag, pos);
			}
			void Room::SetLogicUserPvpRank(Logic::Player* player, const uint32 pvpRank)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			bool Room::PetItemUse(XRated::Logic::Player* player, uint32 id, int bag, int pos, XRated::GlobalSerial serial)
			{
				if (!player) {
					LoggerInstance().Error("Room::PetItemUse() - player == NULL");
					return false;
				}
				Database::Info::ItemInfo* info = Database::DatabaseInstance().InfoCollections.Items.Retrieve(id);
				if (!info)
				{
					LoggerInstance().Error("Room::PetItemUse() - info == NULL");
					return false;
				}
				return m_Logic->PetItemUse(player, info, bag, pos, serial);
			}
			void Room::DropPetItem(XRated::Logic::Player* player, XRated::GlobalSerial serial)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::PetFeed(XRated::Logic::Player* player, const XRated::GlobalSerial& petSerial, const uint32& foodAmount, const uint32& foodCount, bool overFeed)
			{
				if (!player) {
					LoggerInstance().Error("Room::PetFeed() - player == NULL");
					return;
				}
				return m_Logic->PetFeed(player, petSerial, foodAmount, foodCount, overFeed);
			}
			void Room::PetLevelDecrease(Logic::Player* player, const GlobalSerial& petSerial, const uint32& downLevel, const bool& onlyMaxLevel)
			{
				if (!player) {
					LoggerInstance().Error("Room::PetLevelDecrease() - player == NULL");
					return;
				}
				m_Logic->PetLevelDecrease(player, petSerial, downLevel, onlyMaxLevel);
			}
			bool Room::PetItemsEquip(Logic::Player* player, XRated::GlobalSerial petSerial, const Database::Info::ItemInfo* info, Database::Enchant::EnchantBitfields instance, uint32 equipPos)
			{
				LoggerInstance().Exception("Missing implementation");
				return false;
			}
			void Room::TakeCarePetBySchool(Logic::Player* player, const uint32& petItemHash, const XRated::GlobalSerial& petSerial, const Database::Enchant::EnchantBitfields& instance, const uint8& count, const uint32& period, const float& expFactor)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::TakeOutCaredPet(Logic::Player* player, const XRated::GlobalSerial& petSerial)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::PetRenaming(Serial playerSerial, const XRated::GlobalSerial petSerial, const String& newName)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::UnsummonPet(Logic::Player* player, const XRated::Serial playerSerial)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::VoteRequest(UserSharedPtr user, Protocol::ToServer::RequestVoting& requestvoting)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::Vote(UserSharedPtr user, Constants::VoteResult vote)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::PersonalVoteRequest(UserSharedPtr user, Protocol::ToServer::RequestPersonalVoting& requestvoting)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::PersonalVoteClear(UserSharedPtr user, Protocol::ToServer::AnswerPersonalVoting& requestvoting)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::EnterShop(const uint64& serial, Constants::ShopType shop, uint32 param)
			{
				Protocol::FromServer::EnterShop entershop;
				entershop.playerserial = serial;
				entershop.shopnumber = shop;
				entershop.param = param;
				m_UserManager.BroadcastToSerialUsers(entershop);
			}
			void Room::LeaveShop(const uint64& serial, float3 position, float3 direction)
			{
				Protocol::FromServer::LeaveShop leaveshop;
				leaveshop.playerserial = serial;
				leaveshop.position = position;
				leaveshop.direction = direction;
				m_UserManager.BroadcastToSerialUsers(leaveshop);
			}
			void Room::JoinEndUser(UserSharedPtr user, float progress)
			{
				AutoLock lock(m_Mtx);
				Protocol::FromServer::LoadEnd loadEnd;
				loadEnd.charName = user->GetCharacterName();
				loadEnd.progress = progress;
				if (progress >= 1.0f && m_StageInfo) {
					if (m_Locked && m_StageInfo->QuitPlayerAfterLockRoom) {
						LoggerInstance().Error("Room({0})::JoinEndUser - Stage Already Locked", m_RoomIndex);
						Protocol::FromServer::Error error;
						error.errorcode = Errors::UnableToJoinStage;
						user->Send(error);
						user->Close();
						return;
					}
				}
				switch (GetRoomKind()) {
				case Common::STAGE:
					if (progress >= 1.0f) {
						m_UserManager.BroadcastToAllEnteredUsers(loadEnd);
						if (m_Logic->IsLoading()) {
							m_WaitingUsers.push_back(user);
							LoggerInstance().Info("Room({0})::JoinEndUser - Adding user:{1} to the waiting list", m_RoomIndex, user->GetSerial());
						}
						else
							m_Logic->Join(user->GetInitialPlayerData(), std::static_pointer_cast<void>(user));
					}
					break;
				case Common::PVP:
					m_UserManager.BroadcastToAllEnteredUsers(loadEnd);
					if (progress > 1.0f) {
						//I skipped this cause too fucking long
					}
					break;
				case Common::SQUARE:
					user->Send(loadEnd);
					if (progress >= 1.0f)
						m_Logic->Join(user->GetInitialPlayerData(), std::static_pointer_cast<void>(user));
					break;
				}
				auto txtBoard = UserManagerInstance().GetLastTextBoardMsg();
				if (txtBoard != L"") {
					Protocol::FromServer::Chat noticeChat;
					noticeChat.playerserial = user->GetSerial();
					noticeChat.chattype = Constants::ChatTypes::LastTextBoardMsg;
					noticeChat.message = txtBoard;
					user->Send(noticeChat);
				}
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
			const Database::Info::StageGroup* Room::GetCurrentStageGroupInfo() const
			{
				LoggerInstance().Exception("Missing implementation");
				return nullptr;
			}
			StageLocation Room::GetCurrentStage() const
			{
				LoggerInstance().Exception("Missing implementation");
				return StageLocation();
			}
			void Room::GiveUpRevive(Logic::Player* player)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::DecreaseSkillPoint(Logic::Player* player, uint32 skillGroupHash)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::ResetSkillPoint(Logic::Player* player, uint32 skillGroupHash)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			UserSharedPtr Room::GetUser(const uint64& serial)
			{
				LoggerInstance().Exception("Missing implementation");
				return UserSharedPtr();
			}
			void Room::SendToAll(Serializer::ISerializable& packet)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::AddExp(XRated::Constants::ExpAcquiredType type, UserSharedPtr user, uint32 exp, bool withFactor)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::AddPvpExp(XRated::Constants::ExpAcquiredType type, UserSharedPtr user, uint32 exp)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::AddWarExp(XRated::Constants::ExpAcquiredType type, UserSharedPtr user, uint32 exp)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::AddStateBundle(UserSharedPtr user, uint32 hash)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::AddPassive(UserSharedPtr user, uint32 hash)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::RemovePassive(UserSharedPtr user, uint32 hash)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::FamiliarCommand(UserSharedPtr user, uint16 index, XRated::Serial who, Constants::Familiar::Command command)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::ChangeFamiliarFormation(UserSharedPtr user, Constants::Familiar::Formation formation)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			bool Room::IsMissionCleared() const
			{
				LoggerInstance().Exception("Missing implementation");
				return false;
			}
			bool Room::IsNowCampfire() const
			{
				LoggerInstance().Exception("Missing implementation");
				return false;
			}
			float Room::GetObjectDistance(UserSharedPtr user, uint32 hash) const
			{
				LoggerInstance().Exception("Missing implementation");
				return 0.0f;
			}
			void Room::CreateItem(UserSharedPtr user, const XRated::RewardItem& rewardItem, bool isPrivateItem)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			int Room::GetProprietyLevel()
			{
				LoggerInstance().Exception("Missing implementation");
				return 0;
			}
			void Room::ChangeStylePointStateToLogic(Logic::Player* player, XRated::StylePoint::State state)
			{
				m_Logic->ChangeStylePointState(player, state);
			}
			void Room::CashItemView(UserSharedPtr user, std::pair<uint16, uint16> flag)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			std::pair<uint16, uint16> Room::GetCashItemViewFlag(const UserSharedPtr user)
			{
				LoggerInstance().Exception("Missing implementation");
				return std::pair<uint16, uint16>();
			}
			int Room::GetCashItemViewCnt() const {
				return m_UserManager.GetCashItemViewCnt(); 
			}
			void Room::ClearCashItemViewList() { 
				m_UserManager.ClearCashItemViewList();
			}
			void Room::SendReinforcementResult(UserSharedPtr user, ItemPosition itemPos, uint32 hash, int64 oldInstance, int64 newInstance)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::SendLightReinforcementResult(UserSharedPtr user, ItemPosition itemPos, uint32 hash, int64 oldInstance, int64 newInstance)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			uint32 Room::GetRoomIndex() const
			{
				LoggerInstance().Exception("Missing implementation");
				return uint32();
			}
			uint16 Room::GetRoomID() const
			{
				LoggerInstance().Exception("Missing implementation");
				return uint16();
			}
			void Room::RequestFishingList(UserSharedPtr user)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			Database::Info::StageInfo* Room::GetStageInfo() const
			{
				return m_StageInfo;
			}
			void Room::FishingInfo(UserSharedPtr user, uint32 hash, uint8 baitCnt, int32 rare)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::AddFishingUser(uint32 serial)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::RemoveFishingUser(uint32 serial)
			{
				LoggerInstance().Info("RemoveFishingUser Missing implementation");
			}
			int Room::GetFishingUserCnt() const
			{
				LoggerInstance().Exception("Missing implementation");
				return 0;
			}
			void Room::SendAllFishingUser(UserSharedPtr user)
			{
				//LoggerInstance().Exception("Missing implementation");
			}
			void Room::StageGiveUpLog(UserSharedPtr user)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::SlimeRaceBet(UserSharedPtr user, Protocol::ToServer::Gamble::SlimeRace::Bet& packet)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::SlimeRaceClearBet(UserSharedPtr user, Protocol::ToServer::Gamble::SlimeRace::ClearBet& packet)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::SlimeRaceRequestBettingState(UserSharedPtr user)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::SlimeRaceRequestRecentResults(UserSharedPtr user)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::SetPlayTimePenalty(Logic::Player* player, XRated::Constants::PlayTimePenalty::Type flag)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::ChangedExpFactorFromItem(UserSharedPtr user, XRated::Constants::ExpFactorCategoryFromItem category, float factor)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::ChangedExpFactor(UserSharedPtr user)
			{
				Logic::Player* player = user->GetPlayer();
				if (player) {
					m_Logic->SetPlayerExpFactor(player, user->GetExpFactor());
				}
			}
			void Room::AddGuildUser(UserSharedPtr user)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::RemoveGuildUser(UserSharedPtr user)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			bool Room::Rebirth(const UserSharedPtr user, uint32 levelAfterRebirth)
			{
				LoggerInstance().Exception("Missing implementation");
				return false;
			}
			bool Room::RebirthRollBack(const UserSharedPtr user)
			{
				LoggerInstance().Exception("Missing implementation");
				return false;
			}
			void Room::NotifyRebirth(XRated::Serial serial, uint16 level, uint16 rebirthCount, uint16 storedLevel, uint16 storedSkillPoint, const std::vector<XRated::StageLicense>& updatedLicense, const DateTime& lastRebirthDateTime)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			Protocol::FromServer::Family::InviteResult::Type Room::RequestInviteFamily(XRated::Family::FamilySerial familySerial, DateTime createdDate, XRated::Serial targetSerial, const String& targetName, const String& invter)
			{
				LoggerInstance().Exception("Missing implementation");
				return Protocol::FromServer::Family::InviteResult::Type();
			}
			Protocol::FromServer::Family::InviteResult::Type Room::RequestInviteFamily(XRated::Serial targetSerial, const String& targetName, const String& invter)
			{
				LoggerInstance().Exception("Missing implementation");
				return Protocol::FromServer::Family::InviteResult::Type();
			}
			bool Room::CompletedQuest(UserSharedPtr user, uint32 questHash)
			{
				LoggerInstance().Exception("Missing implementation");
				return false;
			}
			bool Room::AcceptedQuest(UserSharedPtr user, uint32 questHash)
			{
				LoggerInstance().Exception("Missing implementation");
				return false;
			}
			uint32 Room::ExcuteRoomSerialUsers(const IExcuter& excuter)
			{
				return uint32();
			}
		}
	}
}