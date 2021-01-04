#include <StageServer/Room/Room.h>
#include <Core/ErrorDefinition.h>
#include <StageServer/User/UserManager.h>
namespace Lunia {
	namespace XRated {
		namespace StageServer {
			//IUserRoom Implementation
			void Room::DebugCommand(User& user, const String& msg)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::SpectatorChat(const String& characterName, Protocol::ToServer::Chat& chat)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::Chat(const uint64& serial, Protocol::ToServer::Chat& chat)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::Voice(const uint64& serial, Protocol::ToServer::Voice& voice)
			{
				LoggerInstance().Exception("Missing implementation");
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
				LoggerInstance().Exception("Missing implementation");
				return false;
			}
			void Room::Cast(XRated::Logic::Player* player, uint32 skill)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::LogicSkillPointUp(XRated::Logic::Player* player, Protocol::ToServer::SetSkillLevel& setskilllevel)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::AddSkillPointPlus(XRated::Logic::Player* player, uint16 skillPointPlus)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::Revive(XRated::Logic::Player* player, bool self)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::DailyRevive(XRated::Logic::Player* player)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::InstantCoinRevive(XRated::Logic::Player* player, uint64 orderNumber)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::CreatePet(XRated::Logic::Player* player, const XRated::Pet& pet)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::GivePresentToPet(XRated::Logic::Player* player, XRated::GlobalSerial petSerial, uint32 sellPrice, uint32 count)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::addminimapping(const uint64& serial, float3 posotion)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			bool Room::Equip(XRated::Logic::Player* player, const Database::Info::ItemInfo* info, Database::Enchant::EnchantBitfields instance, XRated::Constants::Equipment pos)
			{
				LoggerInstance().Exception("Missing implementation");
				return false;
			}
			bool Room::EquipToEquip(Logic::Player* player, XRated::Constants::Equipment from, XRated::Constants::Equipment to)
			{
				LoggerInstance().Exception("Missing implementation");
				return false;
			}
			bool Room::SwapEquip(Logic::Player* player, const std::vector<EquippedItem>& newEquipments)
			{
				LoggerInstance().Exception("Missing implementation");
				return false;
			}
			bool Room::SwapCashEquip(Logic::Player* player, const std::vector<EquippedItem>& newEquipments)
			{
				LoggerInstance().Exception("Missing implementation");
				return false;
			}
			void Room::ValidateEquippedItems(Logic::Player* player)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			bool Room::Use(XRated::Logic::Player* player, uint32 id, int bag, int pos)
			{
				LoggerInstance().Exception("Missing implementation");
				return false;
			}
			void Room::SetLogicUserPvpRank(Logic::Player* player, const uint32 pvpRank)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			bool Room::PetItemUse(XRated::Logic::Player* player, uint32 id, int bag, int pos, XRated::GlobalSerial serial)
			{
				LoggerInstance().Exception("Missing implementation");
				return false;
			}
			void Room::DropPetItem(XRated::Logic::Player* player, XRated::GlobalSerial serial)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::PetFeed(XRated::Logic::Player* player, const XRated::GlobalSerial& petSerial, const uint32& foodAmount, const uint32& foodCount, bool overFeed)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::PetLevelDecrease(Logic::Player* player, const GlobalSerial& petSerial, const uint32& downLevel, const bool& onlyMaxLevel)
			{
				LoggerInstance().Exception("Missing implementation");
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
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::LeaveShop(const uint64& serial, float3 position, float3 direction)
			{
				LoggerInstance().Exception("Missing implementation");
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
			void Room::AddExp(XRated::Constants::ExpAcquiredType type, User& user, uint32 exp, bool withFactor)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::AddPvpExp(XRated::Constants::ExpAcquiredType type, User& user, uint32 exp)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::AddWarExp(XRated::Constants::ExpAcquiredType type, User& user, uint32 exp)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::AddStateBundle(User& user, uint32 hash)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::AddPassive(User& uer, uint32 hash)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::RemovePassive(User& user, uint32 hash)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::FamiliarCommand(User& user, uint16 index, XRated::Serial who, Constants::Familiar::Command command)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::ChangeFamiliarFormation(User& user, Constants::Familiar::Formation formation)
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
			float Room::GetObjectDistance(User& user, uint32 hash) const
			{
				LoggerInstance().Exception("Missing implementation");
				return 0.0f;
			}
			void Room::CreateItem(User& user, const XRated::RewardItem& rewardItem, bool isPrivateItem)
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
				LoggerInstance().Exception("Missing implementation");
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
				LoggerInstance().Exception("Missing implementation");
				return nullptr;
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
				LoggerInstance().Exception("Missing implementation");
			}
			int Room::GetFishingUserCnt() const
			{
				LoggerInstance().Exception("Missing implementation");
				return 0;
			}
			void Room::SendAllFishingUser(UserSharedPtr user)
			{
				LoggerInstance().Exception("Missing implementation");
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
			void Room::ChangedExpFactorFromItem(User& user, XRated::Constants::ExpFactorCategoryFromItem category, float factor)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::ChangedExpFactor(User& user)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::AddGuildUser(User& user)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			void Room::RemoveGuildUser(User& user)
			{
				LoggerInstance().Exception("Missing implementation");
			}
			bool Room::Rebirth(const User& user, uint32 levelAfterRebirth)
			{
				LoggerInstance().Exception("Missing implementation");
				return false;
			}
			bool Room::RebirthRollBack(const User& user)
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
			bool Room::CompletedQuest(User& user, uint32 questHash)
			{
				LoggerInstance().Exception("Missing implementation");
				return false;
			}
			bool Room::AcceptedQuest(User& user, uint32 questHash)
			{
				LoggerInstance().Exception("Missing implementation");
				return false;
			}
			uint32 Room::ExcuteRoomSerialUsers(IExcuter& excuter)
			{
				return uint32();
			}
		}
	}
}