#ifndef IUserRoom_H
#define IUserRoom_h
#pragma once
#include <StageServer/StageServerProtocol/StageServerProtocol.h>
#include <StageServer/Common.h>
#include <Logic/Object/Player.h>

namespace Lunia {
	namespace XRated {
		namespace StageServer {
			class User;

			/*
			class IUserRoom
			{
			public:
				virtual ~IUserRoom() {}

			public:
				virtual void DebugCommand(User& user, const std::wstring& msg) = 0;
				virtual void SpectatorChat(const std::wstring& characterName, Protocol::Chat& chat) = 0;
				virtual void Chat(const uint64& serial, Protocol::Chat& chat) = 0;
				virtual void Voice(const uint64& serial, Protocol::Voice& voice) = 0;
				virtual void PartyInvite(User* user, const uint64& serial) = 0;
				virtual void PartyAccept(User* accepter, User* inviter) = 0;
				virtual void PartyLeave(User* user) = 0;
				virtual void NotifySpectatorLeft(User* user) = 0;
				virtual bool Command(XRated::Logic::Player* player, XRated::Constants::Command command, XRated::Constants::Direction dir) = 0; ///< @reutrn false if the command is invalid
				virtual void Cast(XRated::Logic::Player* player, uint32 skill) = 0;
				virtual void LogicSkillPointUp(XRated::Logic::Player* player, Protocol::SetSkillLevel& setskilllevel) = 0;
				virtual void AddSkillPointPlus(XRated::Logic::Player* player, uint16 skillPointPlus) = 0;
				virtual void Revive(XRated::Logic::Player* player, bool self = true) = 0;
				virtual void DailyRevive(XRated::Logic::Player* player) = 0;
				virtual void InstantCoinRevive(XRated::Logic::Player* player, uint64 orderNumber) = 0;

				virtual void CreatePet(XRated::Logic::Player* player, const XRated::Pet& pet) = 0;
				virtual void GivePresentToPet(XRated::Logic::Player* player, XRated::GlobalSerial petSerial, uint32 sellPrice, uint32 count) = 0;

				virtual void addminimapping(const uint64& serial, float3 posotion) = 0;
				virtual bool Equip(XRated::Logic::Player* player, const Database::Info::ItemInfo* info, Database::Enchant::EnchantBitfields instance, XRated::Constants::Equipment pos) = 0;
				virtual bool EquipToEquip(Logic::Player* player, XRated::Constants::Equipment from, XRated::Constants::Equipment to) = 0;
				virtual bool SwapEquip(Logic::Player* player, const std::vector< EquippedItem >& newEquipments) = 0;
				virtual bool SwapCashEquip(Logic::Player* player, const std::vector< EquippedItem >& newEquipments) = 0;
				virtual void ValidateEquippedItems(Logic::Player* player) = 0;
				virtual bool Use(XRated::Logic::Player* player, uint32 id, int bag, int pos) = 0;
				virtual void SetLogicUserPvpRank(Logic::Player* player, const uint32 pvpRank) = 0;

				virtual bool PetItemUse(XRated::Logic::Player* player, uint32 id, int bag, int pos, XRated::GlobalSerial serial) = 0;
				virtual void DropPetItem(XRated::Logic::Player* player, XRated::GlobalSerial serial) = 0;
				virtual void PetFeed(XRated::Logic::Player* player, const XRated::GlobalSerial& petSerial, const uint32& foodAmount, const uint32& foodCount, bool overFeed = false) = 0;
				virtual void PetLevelDecrease(Logic::Player* player, const GlobalSerial& petSerial, const uint32& downLevel, const bool& onlyMaxLevel) = 0;
				virtual bool PetItemsEquip(Logic::Player* player, XRated::GlobalSerial petSerial, const Database::Info::ItemInfo* info, Database::Enchant::EnchantBitfields instance, uint32 equipPos) = 0;

				virtual void TakeCarePetBySchool(Logic::Player* player
					, const uint32& petItemHash, const XRated::GlobalSerial& petSerial
					, const Database::Enchant::EnchantBitfields& instance, const uint8& count
					, const uint32& period, const float& expFactor) = 0;
				virtual void TakeOutCaredPet(Logic::Player* player, const XRated::GlobalSerial& petSerial) = 0;
				virtual void PetRenaming(Serial playerSerial, const XRated::GlobalSerial petSerial, const std::wstring& newName) = 0;
				virtual void UnsummonPet(Logic::Player* player, const XRated::Serial playerSerial) = 0;

				virtual void VoteRequest(User* user, Protocol::RequestVoting& requestvoting) = 0;
				virtual void Vote(User* user, Constants::VoteResult vote) = 0;
				virtual void PersonalVoteRequest(User* user, Protocol::RequestPersonalVoting& requestvoting) = 0;
				virtual void PersonalVoteClear(User* user, Protocol::AnswerPersonalVoting& requestvoting) = 0;
				virtual void EnterShop(const uint64& serial, Constants::ShopType shop, uint32 param) = 0;
				virtual void LeaveShop(const uint64& serial, float3 position, float3 direction) = 0;
				virtual void JoinEndUser(User* user, float progress) = 0;
				virtual Common::ROOMKIND GetRoomKind() const = 0;
				virtual const Database::Info::StageGroup* GetCurrentStageGroupInfo() const = 0;
				virtual StageLocation GetCurrentStage() const = 0;
				virtual void GiveUpRevive(Logic::Player* player) = 0;

				virtual void DecreaseSkillPoint(Logic::Player* player, uint32 skillGroupHash) = 0;

				virtual void ResetSkillPoint(Logic::Player* player, uint32 skillGroupHash) = 0;

				virtual User* GetUser(const uint64& serial) = 0;
				virtual void SendToAll(Serializer::ISerializable& packet) = 0;

				virtual void AddExp(XRated::Constants::ExpAcquiredType type, User& user, uint32 exp, bool withFactor) = 0;
				virtual void AddPvpExp(XRated::Constants::ExpAcquiredType type, User& user, uint32 exp) = 0;
				virtual void AddWarExp(XRated::Constants::ExpAcquiredType type, User& user, uint32 exp) = 0;
				virtual void AddStateBundle(User& user, uint32 hash) = 0;
				virtual void AddPassive(User& uer, uint32 hash) = 0;
				virtual void RemovePassive(User& user, uint32 hash) = 0;

				virtual void FamiliarCommand(User& user, uint16 index, XRated::Serial who, Constants::Familiar::Command command) = 0;
				virtual void ChangeFamiliarFormation(User& user, Constants::Familiar::Formation formation) = 0;

				virtual bool IsMissionCleared() const = 0;
				virtual bool IsNowCampfire() const = 0;
				virtual float GetObjectDistance(User& user, uint32 hash) const = 0;
				virtual void CreateItem(User& user, const XRated::RewardItem& rewardItem, bool isPrivateItem = false) = 0;
				virtual int	 GetProprietyLevel() = 0;
				virtual void ChangeStylePointStateToLogic(Logic::Player* player, XRated::StylePoint::State state) = 0;
				virtual void CashItemView(User* user, std::pair< uint16, uint16 > flag) = 0;
				virtual std::pair< uint16, uint16 > GetCashItemViewFlag(const User* user) = 0;
				virtual void SendReinforcementResult(User* user, ItemPosition itemPos, uint32 hash, int64 oldInstance, int64 newInstance) = 0;
				virtual void SendLightReinforcementResult(User* user, ItemPosition itemPos, uint32 hash, int64 oldInstance, int64 newInstance) = 0;

				virtual uint32	GetRoomIndex() const = 0;
				virtual uint16	GetRoomID() const = 0;

				//Fishing
				virtual void RequestFishingList(User* user) = 0;
				virtual Database::Info::StageInfo* GetStageInfo() const = 0;
				virtual void FishingInfo(User* user, uint32 hash, uint8 baitCnt, int32 rare) = 0;
				virtual void AddFishingUser(uint32 serial) = 0;
				virtual void RemoveFishingUser(uint32 serial) = 0;
				virtual int GetFishingUserCnt() const = 0;
				virtual void SendAllFishingUser(User* user) = 0;

				virtual void StageGiveUpLog(User* user) = 0;

				//Gamble - SlimeRace
				virtual void SlimeRaceBet(User* user, Net::Protocol::Stage::ToServer::Gamble::SlimeRace::Bet& packet) = 0;
				virtual void SlimeRaceClearBet(User* user, Net::Protocol::Stage::ToServer::Gamble::SlimeRace::ClearBet& packet) = 0;
				virtual void SlimeRaceRequestBettingState(User* user) = 0;
				virtual void SlimeRaceRequestRecentResults(User* user) = 0;

				virtual void SetPlayTimePenalty(Logic::Player* player, XRated::Constants::PlayTimePenalty::Type flag) = 0;

				virtual void ChangedExpFactorFromItem(User& user, XRated::Constants::ExpFactorCategoryFromItem category, float factor) = 0;
				virtual void ChangedExpFactor(User& user) = 0;
				virtual void AddGuildUser(User& user) = 0;
				virtual void RemoveGuildUser(User& user) = 0;

				/// Rebirth
				virtual bool Rebirth(const User& user, uint32 levelAfterRebirth) = 0;
				virtual bool RebirthRollBack(const User& user) = 0;
				virtual void NotifyRebirth(XRated::Serial serial, uint16 level, uint16 rebirthCount, uint16 storedLevel, uint16 storedSkillPoint, const std::vector<XRated::StageLicense>& updatedLicense, const DateTime& lastRebirthDateTime) = 0;

				virtual Net::Protocol::Stage::FromServer::Family::InviteResult::Type RequestInviteFamily(XRated::Family::FamilySerial familySerial, DateTime createdDate, XRated::Serial targetSerial, const std::wstring& targetName, const std::wstring& invter) = 0;
				virtual Net::Protocol::Stage::FromServer::Family::InviteResult::Type RequestInviteFamily(XRated::Serial targetSerial, const std::wstring& targetName, const std::wstring& invter) = 0;


				virtual bool CompletedQuest(User& user, uint32 questHash) = 0;
				virtual bool AcceptedQuest(User& user, uint32 questHash) = 0;

			public:
				class IExcuter
				{
				public:
					virtual bool Excute(User& in) = 0;
				};
				virtual uint32 ExcuteRoomSerialUsers(IExcuter& excuter) = 0;
			};
			*/
		}
	}
}
#endif // !IUserRoom_h
