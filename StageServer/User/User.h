#ifndef User_H
#define User_H
#pragma once
#include "Network/Tcp/Client.h"
#include <StageServer/Service.h>
#include <StageServer/QuickSlot.h>
#include <Network/Api/Api.h>
#include <StageServer/User/PetData.h>
#include <Logic/Logic.h>
#include <Logic/Object/Player.h>
#include <StageServer/User/Services/ExpFactorManager.h>
#include <StageServer/User/Services/FamilyManager.h>
#include <StageServer/User/Services/FishingManager.h>
#include <StageServer/User/Services/GuildState.h>
#include <StageServer/User/Services/QuestManager.h>
#include <StageServer/User/Services/MailBoxManager.h>
#include <StageServer/Protocol/Protocol.h>
#include <StageServer/Protocol/ToServer.h>
#include <StageServer/Common.h>
#include <StageServer/User/Items/Items.h>
#include <Network/DynamicParser.h>
#include <StageServer/User/Items/ItemEx.h>
namespace Lunia {
	namespace XRated {
		namespace StageServer {
			enum class ErrorLevel {
				Curious,			///< not that critical but it should not happen by normal client.
				Critical,			///< critical, it's doubtful about a sort of hack.
				Unexpected,			///< internal wrong situation
				Pvp
			};
			class IUserRoom;
			class User 
				: public Net::ClientTcp
				, public Items::IEventListener
				, public IPetItemVisitorListener
			{
			public:
				enum STATE { NONE, LOAD, ACTIVE, TRADE, TRADEREADY, SHOP, VOTING, OnReleasing };
			public:
				User(asio::ip::tcp::socket&& socket, const uint32& userId);
				std::shared_ptr<User> shared_from_this();

				void Init();
				void BindPackets();
				void Flush();
				void RoomParted();
				void Terminate();
				void Close();
				void SetCharacterName(const String& name);
				void RoomJoined(std::shared_ptr<IUserRoom> room, StageLicense& targetStage);
				void PlayerCreated(Logic::Player* player);
				void ClearCharacterRewardStateFlags();
				void NoticeStylePointState(const XRated::StylePoint::State& state);
				void AutoSaveUserInfo(const DWORD& nowTime, const uint32& synchronizePlayerDataInMs);
				void SavePlayerData();
				void RebirthCharacter(const XRated::ItemPosition& itemPosition);
				void UpdateExpFactor();
				void LoadStage(const StageLicense& targetStage);
				void MissionClear(const bool& success, Database::Info::StageGroup* group, const uint16& accessLevel);
				void ObjectDestroyed(const uint32& id, const uint8& team);
				void CheckExpiredItems();
				void MailAlarmed();
				void RemoveFishingUser();
				void PlayerDeleted();
				void EnterShop(const Constants::ShopType& type, const uint32& param);
				void InstantCoinRevive(const int16& errorNumber, const uint64& orderNumber);
				void PurchaseCashItem(const int16& errorNumber, const uint64& orderNumber);

				//Setters
				void SetSerial(const uint64& userSerial);
				void SetCurrentStage(const StageLicense& targetStage);
				void SetState(const STATE& state);
				
				void SetMoney(const uint32& money);
				void GiveCharacterLicense(const Constants::ClassType& classType);
				bool AddStageLicense(const StageLicense& stageLicense, bool initial = false);
				void SetExpFactorFromItem(XRated::Constants::ExpFactorCategoryFromItem category, const float& factor);
				void SetFamilyExpFactor(const float& factor);
				void SetPenaltyExpFactor(const bool& penalty);
				void AddSkillPointPlus(const uint16& point);
				void AddSkillLicense(const uint32& hash);
				void SetPlayerInitLife(const uint16& initLife);
				void SetPlayerStylePointState(const StylePoint::State& state);
				void SetUltimateSkillFlag(bool used);

				void RaiseEvent(const std::vector<std::wstring>& eventInfo, const uint32& lowRank, const uint32& highRank, const DateTime& endTime);

				bool UpdateInitialPlayerData();
				void Update(const float& dt);
				void UpdatePlayerInfoByUserData();
				void UpdateStackedPlayTime(const float& dt);
				void UpdateInfos();
				void ClearFamilyExpFactor();
				void SendSystemMail(const std::wstring& receiver, const std::wstring& title, const std::wstring& text, const uint32 attachedMoney, const std::vector<XRated::ItemBasicInfo>& sendItems, const std::wstring& sender);
				Protocol::FromServer::Family::InviteResult::Type RequestInviteFamily(XRated::Family::FamilySerial familySerial, DateTime createdDate, XRated::Serial targetSerial, const std::wstring& targetname);
				Protocol::FromServer::Family::InviteResult::Type RequestInviteFamily(XRated::Serial targetSerial, const std::wstring& targetname);

				Protocol::FromServer::Family::InviteResult::Type RequestedInviteFamily(const std::wstring& ownerName, XRated::Family::FamilySerial familySerial, DateTime createdDate);
				Protocol::FromServer::Family::InviteResult::Type RequestedInviteFamily(const std::wstring& ownerName);

				
				bool HasCharacterLicense(const Constants::ClassType& classType) const;
				bool HasSkillLicense(const uint32& hash);

				bool AuthConnection(const json& result);
				void AuthedConnection(const UserSharedPtr& user, const Net::Answer& answer);
				void Authed(const UserSharedPtr& user, const Net::Answer& answer);
				bool Auth(const json& result);
				bool Auth_CharacterInfos(const json& characterinfo);
				bool Auth_CharacterLicenses(const json& licenses);
				bool Auth_StageLicenses(const json& licenses);
				bool Auth_CharacterRebirth(const json& rebirth);
				bool Auth_SetItemList(const json& items);
				bool Auth_SetBagList(const json& bags);
				bool Auth_SkillLicenses(const json& licenses);				
				bool Auth_Skills(const json& skills);
				bool Auth_QuickSlots(const json& quickslots);
				bool Auth_PetsInfo(const json& petsinfo);
				bool Auth_PetsItems(const json& petsitems);
				bool Auth_PetsTraining(const json& petstraining);

				bool IsConnected() const;
				bool IsAuthenticated() const;
				bool IsAbleToJoinStage(const StageLicense& targetStage) const; // IsEnableJoinToStage
				bool IsAuthed() const;
				bool IsLoaded() const;
				bool IsItemLocked() const;
				bool IsFishing() const;
				bool IsEnoughSlotCountByProduceSkill(const uint32& skill);
				bool IsEnableSkill(const uint32& hash) const;
				//Getters
				uint8			GetTeamNumber() const;
				uint8			GetTotalRank() const;
				uint8			GetTimeRank() const;
				uint8			GetLifeRank() const;
				uint8			GetSecretRank() const;
				uint8			GetAirComboRank() const;
				uint8			GetStylePointRank() const;

				uint16			GetLevel() const;
				uint16			GetPvpLevel() const;
				uint16			GetRoomCapacity() const;//GetStartUserCount
				uint32			GetRoomIndex() const;
				uint32			GetId() const; 
				uint32			GetMoney() const;
				uint32			GetMaxAirCombo() const;
				uint64			GetStageXp() const;
				uint32			GetClearXp() const;
				uint16			GetLife() const;
				uint32			GetSPPoint() const;
				uint16			GetRebirthCount() const;
				uint32			GetBlankSlotCount() const;
				uint32			GetBlankSlotCountOfNormalBag() const;

				const int64&	GetActivationSerial() const;
				const uint64&	GetSerial() const;
				float			GetExpFactor();
				float			GetExtraExpFactor();
				std::wstring	GetExtraExpString();
				std::string		GetRoomPass() const;
				String			GetCharacterName() const;
				std::shared_ptr<IUserRoom>					GetRoom() const;
				PetDatas&									GetPetDatas();
				STATE										GetState() const;
				AllMGuildInfo&								GetGuildInfo();
				const std::vector<XRated::StageLicense>&	GetStageLicenses() const;
				const DateTime&								GetLastRebirthDate() const;
				const DateTime&								GetConnectDateTime() const;
				const DateTime::Date						GetConnectDate() const;
				const DateTime::Time						GetConnectTime() const;
				const DateTime::Week::type					GetConnectDayOfWeek() const;
				Logic::Player*								GetPlayer() const;
				CharacterStateFlags							GetCharacterStateFlags() const;
				StageLicense&								GetCurrentStage();
				GuildState&									GetGuildState();
				Logic::ILogic::PlayerInitialData*			GetInitialPlayerData();

				std::mutex& GetSyncObject();
			public://Guild Related
				void SetGuildInfo(const AllMGuildInfo& info);
				void SetGuildExpFactor(const float& factor);
				void SetRequestedInitGuildInfo(bool request);
				bool GetRequestedInitGuildInfo() const;
				void ChangedGuildLevel(const uint8& guildLevel, const uint32& guildExp);
				void ChangedGuildProfitDate(const DateTime& openDate, const DateTime& closeDate);
				bool IsPartOfGuild() const;
				void ReflashGuildProfit();
				void RemoveGuildAdvantage();
				bool ValidGuildInfoAndRun(const XRated::AllMGuildInfo& info);
				void ShareQuestToParty(const Database::Info::QuestInfo& info, const uint32& questHash);
				void ShareQuest(const std::wstring& shareOwner, const Database::Info::QuestInfo& info, const uint32& questHash);
				bool ClearGuildInfo();
				struct ValidGuildInfo {
					uint8	GuildLevel;
					enum Requested {
						None = 0,
						EnterGuildShop,
					};
					Requested	type;
				};
				ValidGuildInfo m_ValidGuildInfo;
			public://Quests
				void CompletedQuest(const uint32& questHash);
				void AcceptedQuest(const uint32& questHash);
			public://Fishing
				void UpdateFishing(const float& dt);
				uint32 GetFishingRodId() const;
				const Database::Info::FishingRodInfo::RodInfo* GetFishingRod() const;
				bool IsProperFishingRod(const uint32& rodHash) const;
				bool IsFishingArea() const;
				void ToggleFishingRodVisibility(const bool& state);
				bool GetFishingRodVisibility() const;
			public:
				/* game logical commands */
				void AddExp(const XRated::Constants::ExpAcquiredType& type, const uint32& exp, const bool& withFactor);
				void AddPvpExp(const XRated::Constants::ExpAcquiredType& type, const uint32& exp);
				void AddWarExp(const XRated::Constants::ExpAcquiredType& type, const uint32& exp);

				void AddStateBundle(const uint32& hash);
				void AddPassive(const uint32& hash);
				void RemovePassive(const uint32& hash);
				float GetObjectDistance(const uint32& targetHash);
			public: //Pets
				void AddTradedPet(const XRated::Pet& pet);
				void RemoveTradedPet(const XRated::GlobalSerial& petSerial);
				bool IsItemInPetInventory(const GlobalSerial& petSerial);
				void CreatePet(const String& petName, const GlobalSerial& petSerial, const uint32& petHash);
				void SummonPet(const uint64& petSerial, const uint8& bag, const uint8& position);
				void UnsummonPet(void);
				void SendUpdatedPetInfo(const XRated::Pet& petData, const XRated::Serial& playerSerial, const XRated::Serial& petSerial, const std::wstring& petExpression);
				void SendPetDeleted(const XRated::GlobalSerial& petSerial);
				Protocol::FromServer::PetUpdated GetUpdatedPetInfo(const XRated::Pet& petData, const XRated::Serial& playerSerial, const XRated::Serial& petSerial, const std::wstring& petExpression);
				void PetChange(const uint32 petItemHash, const XRated::GlobalSerial& petItemSerial);
				void PetOwnerChange(const XRated::GlobalSerial& petItemSerial);
				bool PetFeed(const GlobalSerial& petSerial, const uint32& foodAmount, const uint32& foodCount, bool overFeed);
				bool GivePresentToPet(const GlobalSerial& petSerial, const uint32& sellPrice, const uint32& count);
				bool PetLevelDecrease(const GlobalSerial& petSerial, const uint32& downLevel, const bool& onlyMaxLevel);
				void TakeCarePetBySchool(const uint32& petItemHash, const GlobalSerial& petSerial
					, const Database::Enchant::EnchantBitfields& instance, const uint8& count
					, const uint32& period, const float& expFactor, const uint32& bag, const uint32& pos, uint32 schoolTicketHash);
				void PetRename(const XRated::GlobalSerial petSerial, const std::wstring& newName);
			public: //Item Related.
				void LockItems(bool state);
				uint16 ItemEquiped(bool succeed);
				int GetRequiredSlotCount(const std::vector< std::pair<uint32/*item hash*/, uint32/*count*/> >& toRemove, const std::vector< std::pair<uint32/*item hash*/, uint32/*count*/> >& toAdd, const uint32& availablecount = 1) const; ///<@ return negative value if the removing item slot is more than the adding item slot

				uint32 GetRemainStackatbleItem(const Database::Info::ItemInfo* info, const InstanceEx& instance, const uint32& count) const;
				const Common::ItemEx* GetItem(const ItemPosition& position) const;
				void GetActivatedPassiveItems(std::vector<uint32>& result) const;
				std::pair<uint32, int64> GetEquipment(const uint16& position) const;
				bool ItemExamineSlot(const uint32& id, const int& cnt) const; ///< @remark if the parameter-id is 0, returns true if this user has empty slot.
				uint16 ItemExamineSlot(const XRated::ItemPosition& pos, const uint32& id, const int& cnt) const;
				bool ItemAdd(const uint32& id, Serial gameObjectSerial, const int& cnt, const InstanceEx& instance = InstanceEx(0), bool autoStack = true, XRated::Constants::AddItemType addItemType = XRated::Constants::AddItemType::NoneAddItemType);
				uint32 ItemFill(const uint32& hash, const uint32& amount);///< @return uint32 remaining item amount
				ItemPosition ItemAdd(const Common::ItemEx& item); ///< @return ItemPosition::Invalid If it fails
				ItemPosition ItemAdd(const XRated::Item& item, const uint16& count); ///< @return ItemPosition::Invalid If it fails
				
				bool ItemAcquire(const uint32& itemId, Serial serial, const uint16& stackCount = 1, const InstanceEx& instance = 0, const uint64& itemSerial = 0, bool addSeparate = false, XRated::Item* identifiedAcquireItem = NULL);
				bool ItemAcquireWithMailSupport(const uint32& itemId, const uint16& stackCount = 1, const InstanceEx& instance = 0, const uint64& itemSerial = 0, bool addSeparate = false);
				
				uint32 GetItemCount(const Database::Info::ItemInfo::IFindCondition& condition, bool findPetInven = false) const;
				uint32 GetItemCount(const uint32& hash) const;
				uint32 GetEmptyItemSlotCount() const; ///< @return count of empty inventory slot.

				void ItemDrop(const ItemPosition& pos);
				void DropItems(const uint32& bag, const uint32& pos, const uint32& count);
				bool RemoveStageItem(); ///< @return true when one or mpre stage item has been removed
				bool RemoveItems(const uint32& itemHash, const uint32& count); //using dropItem packet, equipment cannot be removed by this method. return false when failed to drop as count.
				
				bool ItemRemove(const uint32& id, int cnt); ///< @remark Protocol::Stage::FromServer::DropItem packet will be sent automatically against ItemDrop() method
				uint16 ItemRemove(const ItemPosition& position, const uint32& id, const uint16& cnt, bool sendPacket = true, bool writeLog = true); ///<@remark Remove specified items in specified slot. @return Return removed item count.

				void DoRestoreBelonging(const XRated::ItemPosition& usingPos, const XRated::ItemPosition& targetPos);
				void DoAppraisal(const XRated::ItemPosition& usingPos, const XRated::ItemPosition& targetPos);
				void DoCashAppraisal(const XRated::ItemPosition& usingPos, const XRated::ItemPosition& targetPos);
				void DoRestoreAppraisal(const XRated::ItemPosition& usingPos, const XRated::ItemPosition& targetPos);
				void DoCashRestoreAppraisal(const XRated::ItemPosition& usingPos, const XRated::ItemPosition& targetPos);
				void DoExtract(const XRated::ItemPosition& usingPos, const XRated::ItemPosition& targetPos);
			public: // Items::IEventListener
				//bool IsAchievementConnected();
				const std::wstring& GetName() const;
				void ItemsSend(Protocol::IPacketSerializable& packet);
				//void ItemsSendAchievement(Serializer::ISerializable& packet);
				//void ItemAcquiredAchievement(Net::Protocol::Achievement::ServerProtocol::ItemAcquired packet);
				void ItemsSendToAll(Protocol::IPacketSerializable& packet);
				void ItemsDirectSend(Protocol::IPacketSerializable& packet);
				bool ItemsUse(const uint32& itemHash, const uint32& bag, const uint32& pos);

				bool PetItemUse(const uint32& itemHash, const uint32& bag, const uint32& pos, GlobalSerial itemSerial);
				void PetInventoryDirectSend(Protocol::IPacketSerializable& packet);
				bool PetItemsEquip(XRated::GlobalSerial petSerial, const Database::Info::ItemInfo* info, const Database::Enchant::EnchantBitfields& instance, const uint32& equipPos);
				bool PetItemPositionUpdate(XRated::GlobalSerial petSerial, const uint8& bag, const uint8& position);
				void RemoveDuplicatedItem(const uint32& hash, const uint64 serial, const uint8& bag, const uint8& pos, const uint32& stackcount, const InstanceEx& instance, int isBankItem, bool insertAgainAfterDelete);///< arcrus. forced character item info. update.
				
				bool PetVisitor(IPetItemVisitor& visitor);
				
				bool ItemsEquip(const Database::Info::ItemInfo* info, const Database::Enchant::EnchantBitfields& instance, const uint32& equipPos);
				bool ItemsEquipToEquip(const uint32& from, const uint32& to);
				bool EquipmentSwap(const std::vector< EquippedItem >& newEquipments);
				bool CashEquipmentSwap(const std::vector< EquippedItem >& newEquipments);
				Database::Info::SkillInfoList::SkillInfo* GetSkillInfo(const uint32& skillGroupHash) const;
				const XRated::Constants::ClassType& GetClassType() const;
				void CriticalError(const char* logMessage, const bool& block = false, const uint32& blockDuration = 86400);
				void SendSystemChat(const std::wstring& message);
				const Database::Info::StageInfo* GetStageInfo() const;
				void SetEquipmentIndex(const uint8& newIndex);
				void SetCashEquipmentIndex(const uint8& newIndex);
				public: // Mail Related
				void SendItemMail(const uint32& itemHash, uint16 stackCount, const InstanceEx& instance);
				void SendRewardMailByGuildContibutedUp();
				void SendRewardMailByGainStageLicense(const XRated::StageLicense& license);
				void SendMailSelf(const std::wstring& title, const std::wstring& text, const uint32& attachedMoney, const std::vector<XRated::ItemBasicInfo>& sendItems, const std::wstring& sender = L"");
				uint16 GetMailCnt() const;
			private:
				FishingManager								m_FishingManager;
			public://Network Related;
				bool IsAdmitable(const uint32& receivedBytes, const float& costFactor = 1.0f);
				void Send(const Protocol::IPacketSerializable& packet) const;
				void Send(const Serializer::ISerializable& packet) const;
				uint32 Parse(uint8* buffer, size_t size);
				void SendToAll(Protocol::IPacketSerializable& packet) const;
			public://Packet handling coming from PacketHandler
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::Stage& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::Alive& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::LoadEnd& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::Join& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::Command& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::Chat& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::ListItem& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::ListQuickSlot& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::EquipSwap& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::Use& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::MoveItem& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::Voice& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::EnterShop& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::LeaveShop& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::CharacterInfo& packet);

				void Dispatch(const UserSharedPtr user, Protocol::ToServer::Cast& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::SetSkillLevel& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::Revive& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::InstantCoinRevive& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::CashEquipSwap& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::StackItem& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::GivePresentToPet& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::MoveItemInventoryToPet& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::MoveItemInPetInven& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::UseItemInPetInven& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::RequestTakeOutCaredPet& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::TakeOutAllPetItems& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::CheckJoinToSecretStage& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::AddMinimapPing& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::DropItem& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::MoveQuickSlot& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::RequestVoting& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::AnswerPersonalVoting& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::RequestPersonalVoting& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::Vote& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::Buy& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::BuyBack& packet); // 3.1 BY ULTIMATE
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::BuyBarterItem& packet); // 3.1 BY ULTIMATE
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::Compose& packet); // 3.1 BY ULTIMATE
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::BuyToGuildShop& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::Sell& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::DropBankItem& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::BankMoneyIn& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::BankMoneyOut& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::QuickSlot& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::Identify& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::Terminate& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::Reinforce& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::RecoverReinforcement& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::RecoverLightReinforcement& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::ResetIndentification& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::LightReinforce& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::AttachMagic& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::Extract& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::RestoreBelonging& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::ListCashItems& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::CashItemMove& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::CashItemRefund& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::CashItemView& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::UseItemEx& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::Alchemy& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::GiveUpRevive& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::Familiar::ChangeFormation& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::Familiar::Command& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::Random& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::RequestReward& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::Rename& packet);
			public://Db callbacks
				void LicenseAquired(const UserSharedPtr& user, const Net::Answer& answer);
				void PetCreated(const UserSharedPtr& user, const Net::Answer& answer);
			public:
				mutable std::mutex 							m_UserMtx;
				mutable std::mutex 							m_PlayerMtx;
			private:
				uint32										m_UserId = 0;
				uint64										m_UserSerial = 0; //database
				STATE										m_State;
				Constants::ShopType							m_EnterShop;
				String										m_CharacterName = L"";
				PetDatas									m_PetDatas;
				std::vector<PetOrigin>						m_OriginPets;
				Logic::Player*								m_Player = nullptr;
				std::shared_ptr<IUserRoom>					m_Room;
				GuildState									m_GuildState;
				ExpFactorManager							m_ExpFactorManager;
				FamilyManager								m_FamilyManager;
				QuestManager								m_QuestManager;
				MailboxManager								m_MailBoxManager;
				Items										m_Items;
				UseExItemManager							m_ItemEx;
			public: //Auth_Publisher
				std::wstring								m_SecuKey = L"";
				std::wstring 								m_UsingLocale = L"";
				std::wstring								m_Name = L"";
				std::string									m_RoomPass = "";
				uint16										m_RoomCapacity = 0;
				uint32										m_RoomIndex = 0;
				uint8										m_TeamNumber = 0;
				int64										m_RoomActivationSerial = 0;
				StageStates									m_StageStateFlags;
				StageLicense								m_LastStage;
				StageLicense								m_CurrentStage;
			public:
				Logic::ILogic::PlayerInitialData			m_PlayerInitialData;
				CharacterStateFlags							m_CharacterStateFlags;
				DateTime									m_CreateDate = DateTime::Infinite;
				DateTime									m_LastLoggedDate = DateTime::Infinite;
				uint32										m_CharacterLicenses = 0;
				std::vector<XRated::StageLicense>			m_StageLicenses = {};
				std::vector<uint32>							m_SkillLicenses = {};
				QuickSlot									m_QuickSlot;
				uint32										m_CurrentStageHash = 0;
				bool										m_Authed = false;
				bool										m_LoadEnded = false;
				bool										m_IsItemLocked = true;
				bool										m_ItemsChanged = false;
				bool										m_CharacterInfoChanged = false;
				bool										m_IsPartOfGuild = false;
				bool										m_IsEnableToJoinNextLicense = false;
				bool										m_DoesHaveGuildInfo = false;
				bool										m_RequestedInitGuildInfoToDB = false;
				bool										m_UsedUltimateSkill = false;
				bool										m_IsFishingRodVisible = false;
				AllMGuildInfo								m_AllMGuildInfo = AllMGuildInfo();
				CharacterRewardStateFlags					m_CharacterRewardStateFlags = 0;
				DWORD										m_LastDataSavedTime = 0;
				DateTime									m_ConnectDateTime = 0;
				DWORD										m_StackedTodayPlayTime = 0;
				DWORD										m_StackedPlayTimeEventCheckTime = 0;
				DWORD										m_ConnectTime = 0;
				const float									playTimeEventCheckIntervalInSec;
			private:
				DynamicParser<std::shared_ptr<User>>		m_Parser;
			};
			typedef std::shared_ptr<User> UserSharedPtr;
			typedef std::weak_ptr<User> UserWeakPtr;
			typedef std::vector<XRated::LobbyPlayerInfo>::iterator CharactersIterator;

			//template<> std::shared_ptr<User> shared_from(const UserSharedPtr derived);
		}
	}
}
#endif // ! User_H