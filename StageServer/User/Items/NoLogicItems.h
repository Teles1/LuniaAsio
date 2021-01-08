#pragma once
#include "Items.h"
#include <StageServer/User/Services/GuildManager.h>
#include <StageServer/User/Services/PlayerStoreManager.h>
#include <StageServer/User/Services/QuestManager.h>

namespace Lunia {
	namespace XRated {
		namespace StageServer {

			class InvalidRuntimeParameterException : public Exception
			{
			public:
				InvalidRuntimeParameterException(const wchar_t* message, bool isCritical = true) : Exception(message), isCritical(isCritical) {}
				bool IsCritical() const { return isCritical; }

			private:
				bool isCritical;
			};


			/**
				no-logic item means no logic related items that can be use. there're two types in no-logic item. one is normal use-ex (active) which can be used
				by player and the other is passive which is used by server when a stage has started (no runtime parameter should be specified).
			*/
			class NonLogicItem
			{
			public:
				virtual const wchar_t* GetTypeName() const = 0; ///< NonLogicItem is based on NOLOGIC state of the item, therefore GetConsumableCount and Use method need item information as a parameter

				 /**
					calculate the number of items which must be consumed and validate the parameter.
					@param info one of items that has NOLOGIC state.
					@remarks throws InvalidRuntimeParameterException by invalid runtime parameters
				 */
				virtual uint16 GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const = 0;

				/**
					use the given NOLOIGC item
					@param info one of items that has NOLOGIC state.
				*/
				virtual void Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) = 0;

				/** returns NULL if a NOLOGIC state or the tyoe value of the state is not found */
				static const wchar_t* GetNoLogicTypeString(const Database::Info::ItemInfo* info);

			private:
				static const Database::Info::ActionInfo::State& GetNoLogicState(const Database::Info::ItemInfo* info);

			public:
				template <typename T> static T GetNoLogicStateValue(const Database::Info::ItemInfo* info, const wchar_t* name, const T& defaultValue)
				{
					const std::map<String, String>& stateParameters = GetNoLogicState(info).params;
					std::map<String, String>::const_iterator i = stateParameters.find(name);
					if (i == stateParameters.end())
						return defaultValue;

					return StringUtil::To<T>(i->second);
				}
			};

			// active items ///////////////////////////////////////////////////////////////////////////////
			class BankBag : public NonLogicItem
			{
			public:
				BankBag(Items::IEventListener& sender, Items& items, const String& characterName);

				const wchar_t* GetTypeName() const;
				uint16 GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const;
				void Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters);

			private:
				/* should be reference of members of User because the user name string changes after construction */
				Items::IEventListener& sender;
				Items& items;
				const String& characterName;
			};

			class InventoryBag : public NonLogicItem
			{
			public:
				InventoryBag(Items::IEventListener& sender, Items& items, const String& characterName);

				const wchar_t* GetTypeName() const;
				uint16 GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const;
				void Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters);

			private:
				/* should be reference of members of User because the user name string changes after construction */
				Items::IEventListener& sender;
				Items& items;
				const String& characterName;
			};

			class SkillReset : public NonLogicItem
			{
			public:
				SkillReset(XRated::Logic::Player*& player, IUserRoom*& room);

				const wchar_t* GetTypeName() const;
				uint16 GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const;
				void Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters);

			private:
				XRated::Logic::Player*& player;
				IUserRoom*& room;
			};

			class SkillResetAll : public NonLogicItem
			{
			public:
				SkillResetAll(XRated::Logic::Player*& player, IUserRoom*& room);

				const wchar_t* GetTypeName() const;
				uint16 GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const;
				void Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters);

			private:
				XRated::Logic::Player*& player;
				IUserRoom*& room;
			};


			class Nothing : public NonLogicItem
			{
			public:
				Nothing();

				const wchar_t* GetTypeName() const;
				uint16 GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const;
				void Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters);
			};


			class EnchantRecover : public NonLogicItem
			{
			public:
				EnchantRecover(User& user, Items& items);

				const wchar_t* GetTypeName() const;
				uint16 GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const;
				void Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters);

				struct TargetFlags
				{
					enum {
						IdentifyOne = 1 << 0,
						IdentifyAll = 1 << 1,
						InvalidIdentify = IdentifyOne | IdentifyAll,
						ReinforcementRecover = 1 << 2,
						ReinforcementReset = 1 << 3,
						InvalidReinforcement = ReinforcementRecover | ReinforcementReset,
						LightReinforcementRecover = 1 << 4,
						LightReinforcementReset = 1 << 5,
						InvalidLightReinforcement = LightReinforcementRecover | LightReinforcementReset,
					};
				};

			private:
				/* should be reference of members of User because the user name string changes after construction */
				User& user;
				Items& items;
			};

			class PlayerStore : public NonLogicItem
			{
			public:
				PlayerStore(User& user, Items& items, PlayerStoreManager& playerStore);

				const wchar_t* GetTypeName() const;
				uint16 GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const;
				void Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters);

			private:
				static const int MinimalNumberOfSlots = 1;

				User& user;
				Items& items;
				PlayerStoreManager& playerStore;
			};

			class CharacterSlot : public NonLogicItem
			{
			public:
				CharacterSlot(User& user);

				const wchar_t* GetTypeName() const;
				uint16 GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const;
				void Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters);

			private: /* DB response */
				void Extended(const UserSharedPtr& user, Net::Answer& answer);

			private:
				User& user;
			};

			class Megaphone : public NonLogicItem
			{
			public:
				Megaphone(User& user);

				const wchar_t* GetTypeName() const;
				uint16 GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const;
				void Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters);

			private:
				User& user;
			};

			class TextBoardMessage : public NonLogicItem
			{
			public:
				TextBoardMessage(User& user);

				const wchar_t* GetTypeName() const;
				uint16 GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const;
				void Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters);

			private:
				User& user;
			};

			class Store : public NonLogicItem
			{
			public:
				Store(User& user);

				const wchar_t* GetTypeName() const;
				uint16 GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const;
				void Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters);

			private:
				User& user;
			};

			class GuildLevelUp : public NonLogicItem
			{
			public:
				GuildLevelUp(User& user, GuildManager& guild);

				const wchar_t* GetTypeName() const;
				uint16 GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const;
				void Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters);
			private:
				/* DB response */
				//void LevelUp(UserSharedPtr& user, Net::Answer& packet);
			private:
				User& user;
				GuildManager& guild;
			};

			class GuildPointUp : public NonLogicItem
			{
			public:
				GuildPointUp(UserSharedPtr& user, GuildManager& guild);
				const wchar_t* GetTypeName() const;
				uint16 GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const;
				void Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters);
				//private:
				//	/* DB response */
				//	void PointUp(UserSharedPtr& user, Net::Answer& packet);
			private:
				UserSharedPtr user;
				GuildManager& guild;

			};

			class GuildExpUp : public NonLogicItem
			{
			public:
				GuildExpUp(UserSharedPtr& user, GuildManager& guild);
				const wchar_t* GetTypeName() const;
				uint16 GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const;
				void Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters);
			private:
				/* DB response */
				//void ExpUp(UserSharedPtr user, Net::Answer& packet);
			private:
				UserSharedPtr user;
				GuildManager& guild;

			};
			class GuildRankingPointUp : public NonLogicItem
			{
			public:
				GuildRankingPointUp(UserSharedPtr& user, GuildManager& guild);
				const wchar_t* GetTypeName() const;
				uint16 GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>&) const;
				void Use(const Database::Info::ItemInfo* info, const std::vector<String>&);
			private:
				UserSharedPtr user;
				GuildManager& guild;

			};
			class CharacterLicense : public NonLogicItem
			{
			public:
				CharacterLicense(User& user);

				const wchar_t* GetTypeName() const;
				uint16 GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const;
				void Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters);
			private:
				User& user;
			};

			class QuestActivity : public NonLogicItem
			{
			public:
				QuestActivity(User& user, QuestManager& quest);

				const wchar_t* GetTypeName() const;
				uint16 GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const;
				void Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters);

			private:
				User& user;
				QuestManager& quest;
			};

			class ReviveStone : public NonLogicItem
			{
			public:
				ReviveStone(User& user);

				const wchar_t* GetTypeName() const;
				uint16 GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const;
				void Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters);

			private:
				User& user;
			};


			class DailyItem : public NonLogicItem
			{
			public:
				DailyItem(User& user, Items& items);

				const wchar_t* GetTypeName() const;
				uint16 GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const;
				/** Use.
					@parameter : vector 3 0: Bag, 1: Position, 2: Select Item Hash
				*/
				void Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters);

			private:
				User& user;
				Items& items;


				/*
				struct DailyItemLastAcquiredDate
				{
					uint32
						Year	: 11,
						Month	: 4,
						Day		: 5;
				};
				bool IsHaveTodayDailyItem(const AllM::DateTime::Date& connectDate,Database::Enchant::NormalBitfields instance) const;
				*/
			};


			class ExtendExpireItemDate : public NonLogicItem
			{
			public:
				ExtendExpireItemDate(User& user, Items& items);

				const wchar_t* GetTypeName() const;
				uint16 GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const;
				void Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters);

			private:
				User& user;
				Items& items;
				mutable bool useable;
			};

			class ForceItemExpiration : public NonLogicItem
			{
			public:
				ForceItemExpiration(User& user, Items& items);

				const wchar_t* GetTypeName() const;
				uint16 GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const;
				void Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters);

			private:
				User& user;
				Items& items;
				mutable bool useable;
			};

			class PetItem : public NonLogicItem
			{
			public:
				PetItem(User& user, Items& items);

				const wchar_t* GetTypeName() const;
				uint16 GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const;
				void Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters);

			private:
				User& user;
				Items& items;
			};

			class PetFood : public NonLogicItem
			{
			public:
				PetFood(User& user, Items& items);

				const wchar_t* GetTypeName() const;
				uint16 GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const;
				void   Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters);

			private:
				User& user;
				Items& items;
			};

			class PetLevelDown : public NonLogicItem
			{
			public:
				PetLevelDown(User& user, Items& items);

				const wchar_t* GetTypeName() const;
				uint16 GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const;
				void   Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters);

			private:
				User& user;
				Items& items;
				mutable bool useable;
			};

			class PetSchoolTicket : public NonLogicItem
			{
			public:
				PetSchoolTicket(User& user, Items& items);

				const wchar_t* GetTypeName() const;
				uint16 GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const;
				void   Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters);

			private:
				User& user;
				Items& items;
				mutable bool useable;
			};

			class PetRenaming
				: public NonLogicItem
			{
			public:
				PetRenaming(User& user, Items& items);

				const wchar_t* GetTypeName() const;
				uint16 GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const;
				void   Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters);

			private:
				User& user;
				Items& items;
				mutable bool useable;
			};

			class CharacterRenaming : public NonLogicItem
			{
			public:
				CharacterRenaming(User& user, Items& items);

				const wchar_t* GetTypeName() const;
				uint16 GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const;
				void Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters);

			private: /* DB response */
				void Renamed(const UserSharedPtr& user, Net::Answer& answer);

			private:
				User& user;
				Items& items;
				XRated::ItemPosition itemPosition;
			};

			class SkillPointPlus : public NonLogicItem
			{
			public:
				SkillPointPlus(User& user);

				const wchar_t* GetTypeName() const;
				uint16 GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const;
				void   Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters);
			private:
				User& user;
			};

			class AddLicense : public NonLogicItem
			{
			public:
				AddLicense(User& user);

				const wchar_t* GetTypeName() const;
				uint16 GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const;
				void Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters);

			private:
				User& user;
				mutable XRated::StageLicense stageLicense;
			};

			////////////////////////////////////////////////////////////////////////////// AddSkillPoint items //
			class AddItemToGuildShop : public NonLogicItem
			{
			public:
				AddItemToGuildShop(UserSharedPtr& user, GuildManager& guild);
				const wchar_t* GetTypeName() const;
				uint16 GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const;
				void Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters);
			private:
				UserSharedPtr user;
				GuildManager& guild;
			};

			class Rebirth : public NonLogicItem
			{
			public:
				Rebirth(User& user);
				const wchar_t* GetTypeName() const;
				uint16 GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const;
				void Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters);
			private:
				User& user;
				mutable bool useable;
			};

			/////////////////////////////////////////////////////////////////////////////// active items //


			// passive items //////////////////////////////////////////////////////////////////////////////
			class AddLife : public NonLogicItem
			{
			public:
				AddLife(XRated::Logic::Player*& player, const IUserRoom* const& room, XRated::CharacterStateFlags& characterStateFlags);

				const wchar_t* GetTypeName() const;
				uint16 GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const;
				void Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters);
			private:
				XRated::Logic::Player*& player;
				const IUserRoom* const& room;
				XRated::CharacterStateFlags& characterStateFlags;
			};

			class ExpFactor : public NonLogicItem
			{
			public:
				ExpFactor(User& user);

				const wchar_t* GetTypeName() const;
				uint16 GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const;
				void Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters);
			private:
				User& user;
			};

			//class PetAppetiteControl : public NonLogicItem
			//{
			//public:
			//	PetAppetiteControl(XRated::Logic::Player*& player);

			//	const wchar_t* GetTypeName() const;
			//	uint16 GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const;
			//	void Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters);
			//private:
			//	XRated::Logic::Player*& player;
			//};

			////////////////////////////////////////////////////////////////////////////// passive items //

			class EventStage : public NonLogicItem
			{
			public:
				EventStage(User& user);

				const wchar_t* GetTypeName() const;
				uint16 GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const;
				void   Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters);

			private:
				User& user;
			};

			class EventDungeonStage : public NonLogicItem
			{
			public:
				EventDungeonStage(User& user);

				const wchar_t* GetTypeName() const;
				uint16 GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const;
				void   Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters);

			private:
				User& user;
			};

			class BookItem : public NonLogicItem
			{
			public:
				BookItem(User& user);

				const wchar_t* GetTypeName() const;
				uint16 GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const;
				void   Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters);

			private:
				User& user;
			};

			class BookPartItem : public NonLogicItem
			{
			public:
				BookPartItem(User& user);

				const wchar_t* GetTypeName() const;
				uint16 GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const;
				void   Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters);

			private:
				User& user;
			};

			class RestoreBelonging : public NonLogicItem
			{
			public:
				RestoreBelonging(User& user, Items& items);

				const wchar_t* GetTypeName() const;
				uint16 GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const;
				void   Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters);

			private:
				User& user;
				Items& items;
			};

			class Appraisal : public NonLogicItem
			{
			public:
				Appraisal(User& user, Items& items);

				const wchar_t* GetTypeName() const;
				uint16 GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const;
				void   Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters);

			private:
				User& user;
				Items& items;
			};

			class CashAppraisal : public NonLogicItem
			{
			public:
				CashAppraisal(User& user, Items& items);

				const wchar_t* GetTypeName() const;
				uint16 GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const;
				void   Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters);

			private:
				User& user;
				Items& items;
			};

			class RestoreAppraisal : public NonLogicItem
			{
			public:
				RestoreAppraisal(User& user, Items& items);

				const wchar_t* GetTypeName() const;
				uint16 GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const;
				void   Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters);

			private:
				User& user;
				Items& items;
			};

			class CashRestoreAppraisal : public NonLogicItem
			{
			public:
				CashRestoreAppraisal(User& user, Items& items);

				const wchar_t* GetTypeName() const;
				uint16 GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const;
				void   Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters);

			private:
				User& user;
				Items& items;
			};

			class Extract : public NonLogicItem
			{
			public:
				Extract(User& user, Items& items);

				const wchar_t* GetTypeName() const;
				uint16 GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const;
				void   Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters);

			private:
				User& user;
				Items& items;
			};

			class AddSkillLicense : public NonLogicItem
			{
			public:
				AddSkillLicense(User& user);

				const wchar_t* GetTypeName() const;
				uint16 GetConsumableCount(const Database::Info::ItemInfo* info, const std::vector<String>& parameters) const;
				void Use(const Database::Info::ItemInfo* info, const std::vector<String>& parameters);

			private:
				User& user;
				mutable Database::Info::SkillInfoList::SkillGroup* skillGroup;
			};

		}
	}
}
