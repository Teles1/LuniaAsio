#pragma once
#include "Actor.h"
#include "../Action/Skill.h"
#include "../Status/StatusCharacter.h"
#include "../Etc/Inventory.h"
#include <Logic/Object/StatCalculator.h>

#include <list>


namespace Lunia { namespace XRated {	namespace Logic {

	class Player : public Actor 
	{
	public :
		struct ReviveStatus
		{
			enum Type
			{
				None,
				Basic,
				Stone,
				Instance,
				Skill,
			};
		};
		enum ItemUseResult
		{
			Success = 0,
			FailInventoryCoolDownNow,
			FailNotAbleToUse,
			FailItemCoolDownNow,
			FailNotEquippedEquipment,
			FailInvalidComboItem,
			Failure,
		};

		struct Familiar {
			Lunia::XRated::Constants::Familiar::Type type;
			NonPlayer* object;
			Familiar(Constants::Familiar::Type tp, NonPlayer* npc) : type(tp), object(npc) {}
			bool operator== (NonPlayer* npc) { return object == npc; }
			bool operator== (const std::wstring& templateName );
		};
		//SetItem
		struct SetItem { 
			std::wstring Id; // SetItem ID
            uint32 Count; // ���� SetItem ����
			struct Reward {
				std::vector<Stat::IState*> states;
				std::vector<Database::Info::StateBundleInfo*> stateBundles;
			};
			std::map<uint32, Reward > Rewards;
			SetItem(std::wstring id, uint32 cCount) : Id(id), Count(cCount) {}
		};
		struct PetEffect
		{
			std::vector<Stat::IState*> states;
		};

		struct PetWithSummonInfo
		{
			XRated::Pet    Pet;
			XRated::Serial PetNPCSerial;

			PetWithSummonInfo() : PetNPCSerial( 0 ){};
		};

		struct PetWithSummonInfoFinder
		{
			class ByPetNPCSerial
			{
			private:
				XRated::Serial serial;

			public:
				ByPetNPCSerial(const XRated::Serial serial)
					: serial(serial)
				{
				}

				bool operator()(const PetWithSummonInfo& petWithSummonInfo)
				{
					return serial == petWithSummonInfo.PetNPCSerial;
				}
			};

			class ByPetSerial
			{
			private:
				XRated::GlobalSerial serial;

			public:
				ByPetSerial(const XRated::GlobalSerial serial)
					: serial(serial)
				{
				}
				bool operator()(const PetWithSummonInfo& petWithSummonInfo)
				{
					return serial == petWithSummonInfo.Pet.PetSerial;
				}
			};
		};

		typedef std::map<XRated::GlobalSerial, PetEffect> PetEffects;
		typedef std::map<XRated::GlobalSerial, PetEffect>::iterator PetEffectItr;

		typedef std::vector<PetWithSummonInfo> Pets;
		typedef Pets::iterator PetsIter;
		
		typedef std::vector<XRated::PetCaredBySchool>           PetsCared;
		typedef std::vector<XRated::PetCaredBySchool>::iterator PetsCaredIter;
	protected :
		Lunia::XRated::PlayerData playerData;
		Lunia::XRated::PlayerData rebirthRollBackPlayerData;
		uint32 rebirthRollBackXp;
		uint16 rebirthRollBackLevel;

		int rebirthRollBackAddStr;
		int rebirthRollBackAddDex;
		int rebirthRollBackAddVit;
		int rebirthRollBackAddInt;

		StatusCharacter playerStatus;

		//�׾����� ���� ������.
		bool bDead;
		bool isEnableNextLicense;
		bool isGainNormal;
		Lunia::XRated::Serial serialTombStone;
		Constants::GhostRules::Type ghostRule;
		float tGhost;

		bool petUnsummonedAfterDie;


		Lunia::XRated::CharacterStateFlags	characterStateFlags;

		Action::SkillManager skillManager;
		Inventory inventory;


		//Familiar
		Constants::Familiar::Formation familiarFormation;
		std::list<Familiar> familiars;


		//pet
		Pets myPets;
		PetEffects petEffects;		
		PetsCared caredPets;


		//SetItem
		std::list<SetItem> setItems;

		//�ʱ� state�� ����.
		Stat::IState* regenHP, *regenMP;
		uint32 regenHPId, regenMPId;

		/* StylePoint */
		XRated::StylePointProcess stylePoint;
		
		/* Multi Hit */
		DWORD	multiHitCheckTimer;
		uint16	multiHitCount;
		uint16	multiHitMaxCount;
		DWORD	multiHitMaxTime;

		Serial	lastHittedPlayer;

		uint8	totalRank;
		uint8	timeRank;
		uint8	lifeRank;
		uint8	secretRank;
		uint8	airComboRank;
		uint8	spRank;	//StylePoint Rank

		uint32	clearXp;
		uint64	stageXp;

		uint32  pvpRank;

		struct AutoActionManager {
			float elapsedTime;
			const Database::Info::ActionInfoManager::AutoActionList* info;
			uint8 currentStep;

			void Init() { info = NULL; elapsedTime=0; currentStep=0; }
			bool IsPlaying() const { return info ? true : false; }
		} autoActionManager;

		XRated::Constants::PlayTimePenalty::Type playTimePenaltyFlag;

		struct LastActionInfo
		{
			bool bExist;
			uint32 actionName;
			Action::Action::IActionParam* actionParam;
			bool bForce;

			LastActionInfo() : bExist(false), actionName(0), actionParam(NULL), bForce(false) {}
			~LastActionInfo() 
			{
				if (actionParam)
					delete actionParam;
				actionParam = NULL;
			}

			void Clear() 
			{
				if ( bExist ) 
				{
					bExist = false;
					if ( actionParam ) delete actionParam;
					actionParam = NULL;
				}
			}
		}lastActionInfo;

	private :
		//bool RemoveSameFamiliar( uint32 hash );	
		bool RemoveSameFamiliar(NonPlayer* object, Constants::Familiar::Type type);

	public:
		Player(Constants::ClassType type);
		virtual	~Player() {}

	public:
		void AddDamage(float, float);
		void AddDamage(float);
		void RemoveDamage(float, float);
		void RemoveDamage(float);

		Lunia::XRated::PlayerData& GetPlayerData() { return playerData; }
		Pets&          GetPlayerPetData() { return myPets; }		
		PetsCared&     GetCaredPetData() { return caredPets; }
		const std::wstring& GetName() const { return objectData.Name; }
		void SetName(const std::wstring& name) { objectData.Name = name; }
		uint16 GetPvpLevel() const { return playerData.PvpLevel; }
		uint16 GetWarLevel() const { return playerData.WarLevel; }
		uint16 GetStoredLevel() const { return playerData.StoredLevel; }
		uint16 GetRebirthCount() const { return playerData.RebirthCount; }

		bool GetPetUnsummonedAfterDie() const { return petUnsummonedAfterDie; }
		void SetPetUnsummonedAfterDie( bool aValue ) { petUnsummonedAfterDie = aValue; }

		void SetExpFactor(float factor) { playerData.expFactor = factor; }
		std::list<Familiar>& GetTamingList() { return familiars; }

		//Unit Specific property
		const Constants::ClassType& GetClassType() const { return playerData.Job; } //�÷��̾� class�� ������ ���� ���� �Լ�.
		int GetClass() const { return (int)GetClassType(); }//���� ����� �ٸ���. Actor.h �� ����.
		
		const Constants::CharacterMajorStatType GetMajorStatusType() const {
			return Database::DatabaseInstance().InfoCollections.Statuses.GetMajorStatType(Constants::GetClassStr(playerData.Job));
		}
		int GetMajorStatus() const { return (int)GetMajorStatusType(); }

		//Life���� �Լ���
		virtual void Die();
		void ResetBundle(); // �׾����� ����/����/�� �� �����ش�.
		bool Revive( const ReviveStatus::Type reviveStatus, bool bSelf, float hp=100, bool bFromTombStone=false, uint64 orderNumber = 0 );
		bool DailyRevive(bool bSelf = true, float hp=100, bool bFromTombStone=false, uint64 orderNumber = 0 );
		uint16 GetLife() const { return playerData.Life; }
		uint16 GetMaxLife() const { return playerData.MaxLife; }
		void ReduceLife(uint16 l=1) { playerData.Life = playerData.Life - l; SetMaxLife(playerData.Life); }
		void IncLife(uint16 l=1) { playerData.Life = playerData.Life + l; SetMaxLife(playerData.Life); }
		void SetMaxLife(uint16 life) { if(playerData.MaxLife < life) playerData.MaxLife = life; }
		uint16 GetUsableBonusLife() const { return std::min<uint16>(playerData.BonusLife,playerData.UsableBonusLifeInStage); }
		void SetBonusLife( uint16 bonusLife ) {  playerData.BonusLife = bonusLife; }
		void SetLastRebirthDateTime( const Lunia::DateTime lastRebirthDateTime ) {  playerData.LastRebirthDateTime = lastRebirthDateTime; }


		/** This function remove all skill information and reload skills. U must call LoadSkill function to reload skill*/
		void LoadSkill(Constants::ClassType type);
		bool SetSkillLevel(uint32 skill, uint8 level = 0);
		/*
		@return If use skillpoint, return value is skill point after setting. Otherwise, return increased skillpoint.
		**/
		int16 IncSkillLevel(uint32 skill, uint8 level, bool bUseSP = true);
		int16 DecSkillLevel(uint32 skill, uint8 level, bool bUseSP = true);
		int16 ResetSkillLevel(uint32 skill);
		void AddSkillPointPlus(uint16 skillPointPlus);
		int GetSkillLevel(uint32 skill) const;
		int GetSkillPoint() const { return playerData.SkillPoint; }

		bool UseSkill(uint32 skillGroup);
		void ResetCoolDown(uint32 serial);
		void ResetSkillGroupCoolDown(uint32 skillGroupHash);

		//Item
		uint32 UseItem(const Database::Info::ItemInfo* info, Constants::GameTypes gameType);
		void UsePetItem( const GlobalSerial& itemSerial );
		void DropPetItem( const GlobalSerial& itemSerial );
		bool Equip(const Database::Info::ItemInfo* info, int64 fields, Constants::Equipment where, uint32& equipmentOld, bool bForce = false);
		bool ChangeEquipToEquip(Constants::Equipment from,Constants::Equipment to);
		bool SwapEquipment( const std::vector< EquippedItem > & newEquipments );
		void ValidateEquippedItems();
		bool IsEnableEquipment(Constants::Equipment where);

		uint32 GetEquipmentId(Constants::Equipment part) {
			return inventory.GetEquipmentId(part);
		}

		/// Rebirth
		bool Rebirth( uint32 levelAfterRebirth );
		void RebirthRollBack();

		virtual void AddEquipSetBonus();
		/*
		Check where player class is pvpitem player class or not
		@return					true if it is delived class from player class.
		**/
		virtual bool IsPlayerPvpItem() const { return false; }
		/*
		Player get pvp item.
		@param	info			item information
		@return					true if player got item
		**/
		virtual bool AcquireItem(Database::Info::ItemInfo*) { return false; }


		virtual	void Initialize(IGameStateDatabase* db, const std::wstring& name, uint16 lv, uint32 xp, uint32 money, uint32 bankMoney, uint16 storedLevel, uint16 rebirthCount
							, uint16 storedSkillPoint, uint32 ladderPoint, uint16 ladderMatchCount, uint32 ladderWinCount, uint32 ladderLoseCount, uint32 achievementScore
							, uint16 life, uint16 bonusLife,  uint16 skillpoint, uint16 addedSkillPointPlus, uint8 t, float expFactor, float hpFactor, float mpFactor, const Lunia::XRated::CharacterStateFlags& characterStateFlags
							, const std::vector<Pet>& pets, const std::vector<PetCaredBySchool>& petCared, const Lunia::DateTime& lastRebirthDateTime);
		void InitSkill(PlayerData& data);
		void LoadStatus();

		virtual bool Update(float dt, IGameStateDatabase* db);

		bool IsCharacter() const { return true; }
		bool IsDead() const { return bDead; }

		bool IsGainNormal() const { return isGainNormal; }

		bool Command(Action::Action::CommandType state, uint32 param);

		bool DoDamage(Object* who, uint32 creatorActionHash, float minDmg, float maxDmg, float damage, Constants::DamageType dmgType, uint32 stateID, Constants::AttackStateType attackType, unsigned int& sFlag);

		void XpGained(XRated::Constants::ExpAcquiredType type, uint64 xp, bool bUsingExpFactor = true, bool bUsingWeightValue = true, Serial beKilledNpc = 0);
		uint16 LevelUp();
		uint16 PvpLevelUp(XRated::Constants::ExpAcquiredType type, int xp, uint32 xpPvp);
		uint16 WarLevelUp(XRated::Constants::ExpAcquiredType type, int xp, uint32 xpWar);

		bool Tamed(Constants::Familiar::Type type, NonPlayer* object);
		void FamiliarDestroyed(NonPlayer* object);
		void NoticeDestroyToFamiliar(bool bSacrifice = false);
		void ControlFamiliar(XRated::Serial targetSerial, uint16 targetIndex, Constants::Familiar::Command command);
		void ChangeFamiliarFormation(Constants::Familiar::Formation formation);
		float3 GetFamiliarPosition(NonPlayer* object);
		void ClearFamiliar();
		void ResetAllFamiliarStatusByPlayerStat();

		inline const Lunia::XRated::CharacterStateFlags& GetCharacterStateFlags() const	{	return characterStateFlags;	}
		//bool IsBalanced() const { return isBalanced; }

		//Action
		bool SetAction(uint32 name, Action::Action::IActionParam* param, bool force = false); 

		/* StylePoint Commands */
		void ChangeStylePointState(XRated::StylePoint::State state);
		void ChangeStylePointPlayerCount(int count);
		uint32 GetStylePoint();
		float GetSytlePointPercentage();

		void AddStylePoint(const XRated::Constants::StylePointType& type, int count = 1);
		void AddAirComboStylePoint(int airComboCount);
		void AddBackStabStylePoint();
		void AddFantasicSaveStylePoint();

		void UpdateStylePoint();
		void AddDownAttackStylePoint();
		void AddMultiHitStylePoint();

		void SetEnableNextLicense(bool isEnableNextLicense);
		void SetGainNormalExp(int minimumStageLevel);

		inline Serial GetLastHittedPlayer() const	{	return lastHittedPlayer;	}

		inline void SetTotalRank(uint8 rank) { this->totalRank = rank; }
		inline void SetTimeRank(uint8 rank) { this->timeRank = rank; }
		inline void SetLifeRank(uint8 rank) { this->lifeRank = rank; }
		inline void SetSecretRank(uint8 rank) { this->secretRank = rank; }
		inline void SetAirComboRank(uint8 rank) { this->airComboRank = rank; }
		inline void SetStylePointRank(uint8 rank) { this->spRank = rank; }
		
		inline uint8 GetTotalRank() { return totalRank; }
		inline uint8 GetTimeRank() { return timeRank; }
		inline uint8 GetLifeRank() { return lifeRank; }
		inline uint8 GetSecretRank() { return secretRank; }
		inline uint8 GetAirComboRank() { return airComboRank; }
		inline uint8 GetStylePointRank() { return spRank; }

		inline uint64 GetStageXp() const { return stageXp; }
		inline uint32 GetClearXp() const { return clearXp; }

		void AddSkillCooldownTime(uint32 skillHash, float amount);

		bool DoAutoLinkAction();
		bool AutoLinkAction(const Database::Info::ActionInfoManager::AutoActionList*);
		void UpdateAutoLinkAction(float dt);
		inline void StopAutoAction() { if ( autoActionManager.IsPlaying() ) autoActionManager.Init();  }

		//PvPRank
		void SetUserPvpRank(uint32 rank);
		void UpdatePvpRankRwward();

		//SetItem
		inline std::list<SetItem>& GetSetItemList() { return setItems; }
		Player::SetItem* GetSetItemInfo( std::wstring Id );
		void AddSetItemInfo( SetItem setItem );

		void SetPlayTimePenalty(XRated::Constants::PlayTimePenalty::Type flag) { playTimePenaltyFlag = flag; }
		uint32 GetPlayTimePenalty() const { return playTimePenaltyFlag; }

		/// Pet
		bool PetSummon(NonPlayer* object);
		bool PetUnsummon(NonPlayer* object);
		bool PetUnsummon(XRated::Pet& pet);
		void UnsummonPet();

		bool PetUpdate(float dt);
		//void PetExpUpdate(XRated::Pet& petData);
		void PetLevelUp(XRated::Pet& pet);

		bool AddPetEffectState(const XRated::Pet& pet, NonPlayer* object);
		bool RemovePetEffectState(const XRated::Pet& pet);

		bool IsMaxPetLevel(const XRated::GlobalSerial& petSerial);
		bool IsRarePet(const XRated::GlobalSerial& petSerial);

		void GiveFoodToPet( XRated::GlobalSerial petSerial, uint32 foodAmount, uint32 foodCount, bool overFeed = false );
		void GivePresentToPet(XRated::GlobalSerial petSerial, uint32 presentSellPrice, uint32 presentItemCount);

		bool RunPetEmotion(const XRated::GlobalSerial& petSerial, bool sendOnlyChanged);

		bool PetEquip(XRated::GlobalSerial petSerial, const Database::Info::ItemInfo* info, int64 fields, Constants::Equipment where, uint32& equipmentOld, Serial& petObjectSerial, bool bForce = false);
		bool ChangePetEquipToEquip(XRated::GlobalSerial petSerial, Constants::Equipment from,Constants::Equipment to);

		void AddPet( const XRated::Pet& pet );

		void TakeCarePetBySchool( const uint32& petItemHash, const XRated::GlobalSerial& petSerial
								, const Database::Enchant::EnchantBitfields& instance, const uint8& count
			                    , const uint32& period, const float& expFactor );
		void TakeOutCaredPet( const XRated::GlobalSerial& petSerial );

		void PetRenaming(XRated::GlobalSerial petSerial, const std::wstring& newName);

		virtual void RecalculateHP(bool bFill, bool bIgnoreLimit);

		virtual void RecalculateMP(bool bFill, bool bIgnoreLimit);

		virtual void RecalculateSecondStat();

		virtual void RecalculateStatus(bool bIgnoreLimit = false);
		virtual void ChangePlayerStatusRate( std::vector<uint32>* statusId, const std::vector<float>& rate );
		virtual void ChangePlayerStatusLimit( std::vector<uint32>* statuses, const std::vector<uint32>& limits );

		virtual float GetSkillDamageIncreaseFactorByStat() const;
		virtual float GetCriticalProbabilityByStat() const;
		virtual float GetCoolDownTimeReduceAmount() const;
		virtual float GetPhysicalCriticalInc() const;
		virtual float GetMagicalCriticalInc() const;
		virtual float GetHitIgnoreHpRate() const;
		virtual float GetHealIncRateByStat() const;
		virtual bool IsHitEffectIgnored() const;

		virtual const float GetHitIgnoreValue() const;

		uint32 ApplyLadderResult( uint32 opponentLadderPoint, bool isWin , XRated::PvpSubInfo::Type subInfo );
		uint32 GetLadderPoint() const;

	private :
		void operator= (Player&);
		
		SecondStat secondStat;
	};

} } }

