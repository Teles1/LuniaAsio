#include <WinSock2.h>
#include "Player.h"
#include <Core/Resource/Xml/Xml.h>
#include <Core/Resource/Resource.h>
#include "../State/States.h"
#include "../State/StateLessMP.h"
#include "../State/StateRegen.h"
#include "../State/StateDevineShd.h"
#include "../State/StateCondition.h"
#include "../State/StateBlockUsingSkill.h"

#include "TombStone.h"
#include <Info/Info/Database.h>
#include "../GameState.h"
#include "../LogicDB/LogicDB.h"
#include "../Stage/Stage.h"

#include <Info/Info/InfoHelper.h>

#include "StatCalculator.h"
#include <Core/Utils/ConfigReader.h>
#include <Info/Info/ETC/Balance.h>

#include <iostream>
#include <iterator>
#include <algorithm>
using namespace std;

using namespace Lunia::Xml;
using namespace Lunia::XRated::Database;

namespace Lunia { namespace XRated {	namespace Logic {

	class StateTypeFinder
	{
	private:
		const Database::Info::StateInfo::Type stateType;

	public:
		StateTypeFinder(const Database::Info::StateInfo::Type stateType)
			: stateType(stateType)
		{
		}

		bool operator()(const Database::Info::ActionInfo::State& rhs)
		{
			return rhs.type == stateType;
		}
	};

	//bool Player::Familiar::operator== (uint32 hash)
	//{
	//	//object->GetTemplateName()
	//	return object->GetNameHash() == hash;
	//}
	bool Player::Familiar::operator== (const std::wstring& templateName )
	{
		return object->GetTemplateName() == templateName;
	}

	Player::Player(Constants::ClassType type)
		:	Actor(Lunia::XRated::Constants::ObjectType::Player), playerData(characterData), rebirthRollBackPlayerData(characterData), skillManager(this), bDead(false), petUnsummonedAfterDie( false ),
			totalRank(XRated::RankF), timeRank(XRated::RankF), lifeRank(XRated::RankF), secretRank(XRated::RankF), airComboRank(XRated::RankF), spRank(XRated::RankF), 
			stylePoint(Lunia::XRated::Database::DatabaseInstance().InfoCollections.StylePoints.Retrieve().StylePointGaugeMinusScore, 
					   Lunia::XRated::Database::DatabaseInstance().InfoCollections.StylePoints.Retrieve().StylePointGaugeMinusTimeInSec,
					   Lunia::XRated::Database::DatabaseInstance().InfoCollections.StylePoints.Retrieve().StyleProintStartProcessGab)

	{
		myPets.reserve(30);
		playerData.Job = type;
		playerData.expFactor = 0;
		playerData.bDead = false;		
	}


	bool Player::RemoveSameFamiliar(NonPlayer* object, Constants::Familiar::Type type)
	{
		for(std::list<Familiar>::iterator i = familiars.begin(); i != familiars.end() ; ++i)
		{
			if(i->type == type)
			{
				if((*i) == object->GetTemplateName()) {
					if(type == XRated::Constants::Familiar::Type::Pet) {
						PetUnsummon(i->object);						
					}else {
						i->object->MasterCommandMeSuicide(false);
						familiars.erase(i);
					}
					return true;
				}
			}
		}

		return false;
	}

	void Player::InitSkill(PlayerData& data)
	{
		std::vector<Skill> tmpSkills = data.Skills;

		std::vector<Lunia::XRated::Skill>::iterator i = tmpSkills.begin(), end = tmpSkills.end();
		for (; i != end ; ++i)
			SetSkillLevel( (*i).Id, (*i).Level );
	}


	void Player::Initialize(IGameStateDatabase* db, const std::wstring& name, uint16 lv, uint32 xp, uint32 money, uint32 bankMoney, uint16 storedLevel, uint16 rebirthCount
							, uint16 storedSkillPoint, uint32 ladderPoint, uint16 ladderMatchCount, uint32 ladderWinCount, uint32 ladderLoseCount, uint32 achievementScore
							, uint16 life, uint16 bonusLife, uint16 skillpoint, uint16 addedSkillPointPlus, uint8 t, float expFactor, float hpFactor, float mpFactor, const Lunia::XRated::CharacterStateFlags& characterStateFlags
							, const std::vector<Pet>& pets, const std::vector<PetCaredBySchool>& petCared, const Lunia::DateTime& lastRebirthDateTime)
	{
		Actor::Initialize(db);
		myPets.clear();	
		myPets.reserve(30);

		objectData.Name = name;
		objectData.NameHash = StringUtil::Hash( name.c_str() );
		characterData.Level = lv;
		characterData.Xp = xp;
		characterData.Money = money;
		characterData.BankMoney = bankMoney;
		playerData.Life = life;
		playerData.MaxLife = life;
		playerData.BonusLife = bonusLife;
#ifdef _SINGLE		///by kpongky( 09.07.13 ) to fix problem of single-client link error
		playerData.UsableBonusLifeInStage = 2;
#else
		playerData.UsableBonusLifeInStage = Config::GetInstance().Get<uint16>("UsableBonusLifeInStage");
#endif
		playerData.SkillPoint = skillpoint;		
		playerData.expFactor = expFactor;
		playerData.AddedSkillPointPlus = addedSkillPointPlus;
		playerData.StoredLevel = storedLevel;
		playerData.RebirthCount = rebirthCount;
		playerData.LadderPoint = ladderPoint;
		playerData.LadderMatchCount = ladderMatchCount;
		playerData.LadderWinCount = ladderWinCount;
		playerData.LadderLoseCount = ladderLoseCount;
		playerData.achievementScore = achievementScore;
		playerData.StoredSkillPoint = storedSkillPoint;
		playerData.LastRebirthDateTime = lastRebirthDateTime;
		characterData.Team = t;
		characterData.Alliance = 1;

		this->characterStateFlags = characterStateFlags;
		///this->isBalanced = isBalanced;

		regenHPId = regenMPId = 0;

		LoadStatus();
		LoadSkill(playerData.Job);
		inventory.Init();

		familiarFormation = Constants::Familiar::Formation::Follow;
		familiars.clear();

		tGhost = 0;
		bDead = false;
		this->GetPlayerData().bDead = false;
		serialTombStone = 0;

		if ( hpFactor > 1 )
			AddHPMul(hpFactor - 1);

		if ( mpFactor > 1 )
			AddMPMul(mpFactor - 1);

		/// �ٶ����� üũ �߿��ϴ� �ٶ��� �ٶ��� �ٶ��� /// IsBalanced�� õĪ������ ����.( Pvp ����ä�� )
		if(characterStateFlags.IsBalanced)
		{
			status.Vit += static_cast<int>(Database::BalancerInstance().GetValue(static_cast<XRated::Constants::ClassType>(GetClass()), Database::BalancerInstance().Vit, playerData.BaseCharacter.Level));


		
			status.hpPlus += Database::BalancerInstance().GetValue(static_cast<XRated::Constants::ClassType>(GetClass()), Database::BalancerInstance().Hp, playerData.BaseCharacter.Level);
			RecalculateHP(false, false);
			//status.RecalculateHP(false, false);

			//if(status.MaxHp > Database::BalancerInstance().GetValue(static_cast<XRated::Constants::ClassType>(GetClass()), Database::BalancerInstance().AverageHp))
			//{
			//	status.hpPlus -= (status.MaxHp - Database::BalancerInstance().GetValue(static_cast<XRated::Constants::ClassType>(GetClass()), Database::BalancerInstance().AverageHp));
			//	status.RecalculateHP(false, false);
			//}
		}

		RecalculateHP(false, false);
		//status.RecalculateHP(false, false);
	
		multiHitCheckTimer = 0;
		multiHitCount = 0;
		multiHitMaxCount = static_cast<uint16>(Database::DatabaseInstance().InfoCollections.GeneralInfos.Retrieve().MultiHitCount);
		multiHitMaxTime = Database::DatabaseInstance().InfoCollections.GeneralInfos.Retrieve().MultiHitTimeInMSec;
	
		totalRank = XRated::RankF;
		timeRank = XRated::RankF;
		lifeRank = XRated::RankF;
		secretRank = XRated::RankF;
		airComboRank = XRated::RankF;
		spRank = XRated::RankF;

		stageXp = 0;
		clearXp = 0;

		autoActionManager.Init();

		setItems.clear();

		playTimePenaltyFlag = 0;

		size_t i = 0, size = pets.size();
		for(; i < size; ++i ) {
			AddPet( pets[i] );
		}

		caredPets = petCared;

		petEffects.clear();

		pvpRank = 0;
	}

	void Player::LoadStatus()
	{
		const Info::GeneralInfo::CharacterInfo& charInfo = 
			DatabaseInstance().InfoCollections.GeneralInfos.Retrieve().CharacterInfos.at((int)playerData.Job);

		objectData.Radius = charInfo.Radius;
		playerData.MaxSp = charInfo.MaxSp;
		playerStatus.regenHp = charInfo.RegenHp;
		playerStatus.regenMp = charInfo.RegenMp;

		//�⺻ player������ �ε� �Ѵ�.
		playerStatus.statusGroup = DatabaseInstance().InfoCollections.Statuses.Retrieve(Constants::GetClassStr(playerData.Job));
		playerStatus.info = playerStatus.statusGroup->statusList.at(characterData.Level-1);

		uint32 increaseStatusFactor = DatabaseInstance().InfoCollections.Statuses.GetRebirthAdditionalFactor(characterData.Level, playerData.RebirthCount, true);
		uint32 increaseMajorStatusFactor = DatabaseInstance().InfoCollections.Statuses.GetRebirthAdditionalMajorFactor(characterData.Level, playerData.RebirthCount, true);

		status.Str = playerStatus.info->Strength + increaseStatusFactor;
		status.Dex = playerStatus.info->Dextrity + increaseStatusFactor;
		status.Int = playerStatus.info->Intelligence + increaseStatusFactor;
		status.Vit = playerStatus.info->Vitality + increaseStatusFactor;

		switch (DatabaseInstance().InfoCollections.Statuses.GetMajorStatType(Constants::GetClassStr(playerData.Job)) )
		{
		case Constants::CharacterMajorStatType::Dex:
			status.Dex += (increaseMajorStatusFactor - increaseStatusFactor);
			break;
		case Constants::CharacterMajorStatType::Int:
			status.Int += (increaseMajorStatusFactor - increaseStatusFactor);
			break;
		case Constants::CharacterMajorStatType::Str:
			status.Str += (increaseMajorStatusFactor - increaseStatusFactor);
			break;
		case Constants::CharacterMajorStatType::Vit:
			status.Vit += (increaseMajorStatusFactor - increaseStatusFactor);
			break;
		}

		status.basicHp = playerStatus.info->Hp;
		status.basicMp = playerStatus.info->Mp;
		status.stackedInt = playerStatus.info->Intelligence;
		status.stackedVit = playerStatus.info->Vitality;

		status.dmg.str = status.GetStr();
		status.dmg.dex = status.GetDex();

		regenHP = new Stat::RegenHp(this, Stat::RegenHp::ChargeType::AMOUNT, playerStatus.regenHp, false);
		AddState(regenHP);
		regenHPId = regenHP->GetId();
		regenMP = new Stat::RegenMp(this, Stat::RegenMp::ChargeType::AMOUNT, playerStatus.regenMp, false);
		AddState(regenMP);
		regenMPId = regenMP->GetId();
	}

	void Player::AddDamage( float min, float max )
	{	
		Actor::AddDamage(min, max);

		for (std::list<Familiar>::iterator i =  familiars.begin() ; i != familiars.end() ; ++i )
		{
			if((*i).type != XRated::Constants::Familiar::Type::Pet)
			{
				(*i).object->AddDamage(min, max);
			}
		}
	}

	void Player::AddDamage( float mul )
	{
		Actor::AddDamage(mul);

		for (std::list<Familiar>::iterator i =  familiars.begin() ; i != familiars.end() ; ++i )
		{
			if((*i).type != XRated::Constants::Familiar::Type::Pet)
			{
				(*i).object->AddDamage(mul);
			}
		}
	}

	void Player::RemoveDamage( float min, float max )
	{
		Actor::RemoveDamage(min, max);

		for (std::list<Familiar>::iterator i =  familiars.begin() ; i != familiars.end() ; ++i )
		{
			if((*i).type != XRated::Constants::Familiar::Type::Pet)
			{
				(*i).object->RemoveDamage(min, max);
			}
		}
	}

	void Player::RemoveDamage( float mul )
	{
		Actor::RemoveDamage(mul);

		for (std::list<Familiar>::iterator i =  familiars.begin() ; i != familiars.end() ; ++i )
		{
			if((*i).type != XRated::Constants::Familiar::Type::Pet)
			{
				(*i).object->RemoveDamage(mul);
			}
		}
	}

	void Player::LoadSkill(Constants::ClassType type)
	{
		skillManager.Init(type);
	}

	int Player::GetSkillLevel(uint32 skill) const
	{
		int16 returnValue = skillManager.GetSkillLevel(skill);
		if ( returnValue < 0 )
			return 0;

		return returnValue;
	}

	bool Player::SetSkillLevel(uint32 skill, uint8 level)
	{
		int skillLevel = skillManager.GetSkillLevel(skill);
		if ( skillLevel == -1)	//�ش� ��ų�� ����.
			return false;

		return skillManager.SetSkillLevel(skill, level);
	}

	int16 Player::IncSkillLevel(uint32 skillGroupHash, uint8 level, bool bUseSP)
	{
		if ( bUseSP && playerData.SkillPoint < level) {
			//���� ��ų����Ʈ�� �ʹ� ����.
			return -1;
		}

		uint8 increasedLevel = skillManager.IncSkillLevel(skillGroupHash, level, bUseSP);
		if( increasedLevel ) {
			if ( bUseSP ) {
				playerData.SkillPoint = playerData.SkillPoint - level; // consume the skill point by set the skill
				std::vector<Lunia::XRated::Skill>::iterator i=std::find_if(playerData.Skills.begin(), playerData.Skills.end(), XRated::Skill::Finder(skillGroupHash));
				if (i==playerData.Skills.end()) {
					playerData.Skills.push_back( Lunia::XRated::Skill(skillGroupHash, level) );
					stageData->SkillPointChanged(this, skillGroupHash, skillManager.GetSkillLevel(skillGroupHash));
					return level;
				}

				i->Level = static_cast<uint8>(i->Level + level);

				stageData->SkillPointChanged(this, skillGroupHash, skillManager.GetSkillLevel(skillGroupHash));

				return i->Level;
			} else {//���������� �����Ѱ�. �� �����ص� ��� ����.
				stageData->SkillPointChanged(this, skillGroupHash, skillManager.GetSkillLevel(skillGroupHash));
				return increasedLevel;
			}
		}

		return 0;
	}

	int16 Player::DecSkillLevel(uint32 skillGroupHash, uint8 level, bool bUseSP)
	{
		if ( skillManager.DecSkillLevel(skillGroupHash, level, bUseSP) ) {
			if ( bUseSP ) {
				playerData.SkillPoint = playerData.SkillPoint + level; // recover the skill point
				stageData->SkillPointChanged(this, skillGroupHash, skillManager.GetSkillLevel(skillGroupHash));
				std::vector<Lunia::XRated::Skill>::iterator i=std::find_if(playerData.Skills.begin(), playerData.Skills.end(), XRated::Skill::Finder(skillGroupHash));
				if (i==playerData.Skills.end()) return 0;

				i->Level = static_cast<uint8>(i->Level - level);


				if (i->Level) return i->Level;

				/* reached to 0 */
				playerData.Skills.erase(i);
			} else {
				stageData->SkillPointChanged(this, skillGroupHash, skillManager.GetSkillLevel(skillGroupHash));
			}
		}
		return 0;
	}

	int16 Player::ResetSkillLevel(uint32 skill)
	{
		uint8 skillLv = 0;
		std::vector<Lunia::XRated::Skill>::iterator i = playerData.Skills.begin(), end = playerData.Skills.end();
		for ( ; i != end ; ++i ) {
			if ( (*i).Id == skill ) {
				skillLv = (*i).Level;
				break;
			}
		}
		if ( skillManager.ResetSkillLevel(skill) ) {
			playerData.SkillPoint = playerData.SkillPoint + skillLv;
			//playerData�� skill level ����.
			if ( i != playerData.Skills.end() ) {
				*i = playerData.Skills.back();
				playerData.Skills.pop_back();
			}
		}
		return 0;
	}
	void Player::AddSkillPointPlus(uint16 skillPointPlus)
	{
		playerData.SkillPoint += skillPointPlus;
		playerData.AddedSkillPointPlus += skillPointPlus;
		//room���� �ٽ� �˷��ش�.
		stageData->AddedSkillPointPlus(this, playerData.SkillPoint, playerData.AddedSkillPointPlus, playerData.StoredSkillPoint );
	}

	bool Player::UseSkill(uint32 skillGroup)
	{
		bool bCheckHitAction = false;
		if ( !actions.IsAbleToCast(skillGroup, stageData->GetStageGroupInfo()->GameType) ) {//���� �׼ǻ��°� casting�� ���������� ����.
			if ( actions.GetActionData().bHit ) {//�Ұ����ϴٸ� ������ �� ��ũ�Ǵ� �׼��� ���డ������ �˾ƺ���.
				bCheckHitAction = true;
			} else 
				return false;
		}

		if(CheckState(Database::Info::StateInfo::BLOCKUSINGSKILL) == true)
			return false;

		uint32 actionName;
		float hp = status.Hp; float mp = status.Mp;

		float coolDownFactor = 1.0f - GetCoolDownTimeReduceAmount();

		if ( !skillManager.IsAbleToCast(skillGroup, actionName, coolDownFactor, mp, hp, bCheckHitAction, status.MaxMp, status.MaxHp) ) 
		{
			if ( mp < 0 || hp < 0) //���� Ȥ�� ü�� �������� �ߵ����� ���ߴ�.
				return actions.SetAction(Info::HashTable::Actions::NoMana, this, NULL, *stageData, true);
			return false;
		}

		// ��ų �̸��� ����δ�... ��ų�� ����� ���...
		actions.RemoveReservedAction();
		if ( actions.SetAction(actionName, this, NULL, *stageData, true) ) {
			//if (playerData.Job == Constants::ClassType::Healer) { // ����� �ǵ尡 �ɷ� �ִٸ� ��������.
				Stat::IState* s = GetState(Info::StateInfo::Type::DEVINESHD, Info::StateInfo::Sort::ORDINARYSTATE);
				if (s) ((Stat::DevineShd*)s)->Remove();
			//}

			unsigned int sFlag=0; //passive flag

			//���� ���Ұ� �ߵ��Ǿ��°�.
			if(CheckState(Info::StateInfo::LESSMP) && IsLessMp(stageData) == true)
			{
				/// LessMP State ���� �� ���Mp 1/2 -> 0����~
				///		2008.03.05 Yong-Eun, Jung
				//if(IsLessMp() == true)					
				mp = 0.0f;
			}

			if(stageData->GetStageGroupInfo()->GameType & XRated::Constants::GameTypes::PvpGameTypeMask && GetCharacterStateFlags().IsBalanced)
				hp *= Database::BalancerInstance().GetValue(static_cast<XRated::Constants::ClassType>(GetClass()), Database::IBalancer::DamageFactor, GetLevel());

			//����� ������ ü���� �缳��������.
			float max;
			SetMP(GetMP(max) - mp);
			SetHP(GetHP(max) - hp);

			//Database::Info::SkillInfoList::SkillInfo* info = GetSkillInfo(skillGroup);

			///	Skill�� ������� �� ó��
			if(CheckState(Info::StateInfo::IFCASTSKILL) == true)
			{
				for (StateList::iterator itr = stateBuffers.state_condition.begin(); itr != stateBuffers.state_condition.end(); ++itr)
				{
					Stat::IState* state = (*itr);

					if(state == NULL)
						continue;

					if(state->GetType() == Info::StateInfo::IFCASTSKILL && ((Stat::IfCastSkill*)state)->CheckSkillGroupName(skillGroup))
						state->Do((Object*)NULL, stageData);
				}
			}

			stageData->Casted( objectData.GameObjectSerial, skillGroup, (uint8)skillManager.GetSkillLevel(skillGroup) );
			stageData->StatusChanged(objectData.GameObjectSerial, objectData.Position, objectData.Direction, characterData.Hp, characterData.Mp, objectData.GameObjectSerial, actionName, sFlag);
			return true;
		}
		return false;
	}

	void Player::ResetCoolDown(uint32 serial)
	{
		skillManager.ResetCoolDown();
		stageData->StatusChanged(objectData.GameObjectSerial, objectData.Position, objectData.Direction, characterData.Hp, characterData.Mp
			, serial, 0, Constants::StatusFlag::ResetCooldown);
	}

	void Player::ResetSkillGroupCoolDown(uint32 skillGroupHash)
	{
		if(skillManager.GetSkillLevel(skillGroupHash) > 0)
		{
			skillManager.ResetGroupCoolDown(skillGroupHash);
			stageData->StatusChanged(objectData.GameObjectSerial, objectData.Position, objectData.Direction, characterData.Hp, characterData.Mp
				, GetSerial(), skillGroupHash, Constants::StatusFlag::ResetCooldown);
		}
	}

	uint32 Player::UseItem(const Info::ItemInfo* info, Constants::GameTypes gameType)
	{
		std::cout << "uint32 Player::UseItem(const Info::ItemInfo* info, Constants::GameTypes gameType)" << std::endl;
		if ( CheckState(Database::Info::StateInfo::GHOST) ) {
			if (info->Attributes.Attr & Database::Info::ItemInfo::ATTRIBUTES::USEWHENGHOST) {
				//TODO : Revive�� �ٷ� ���� �� ���� ���ٴ°�. -0-;
				Info::FindReviveState condition(Database::Info::StateInfo::Type::REVIVE);
				if ( condition(*info) && !stageData->GetStage().IsMissionCleared() && playerData.Life == 0 ) { //��Ȱ �������̴�.
					//if( ??? GameStateDatabase->GetTotalReviveCount() >= stageData->GetReviveLimitCount()){ @HERE TELES PLS TY.
					//
					//} else {
						Revive( ReviveStatus::Stone, false);
						return ItemUseResult::Success;
					//}
				} else
					return ItemUseResult::Failure; //TODO : �ϴ� ���ɻ����� �� �ٸ� � �����۵� ��� �� �� ����.
			}
            return ItemUseResult::Failure; //���ɻ��� �϶� ��� �� �� �ִ� �������� �ƴ϶�� �� �� ���°� �翬.
		}
		if ( info->Attributes.Attr & Database::Info::ItemInfo::ATTRIBUTES::USEWHENGHOST )
			return ItemUseResult::Failure;

		if(Database::DatabaseInstance().InfoCollections.Pets.IsPetItem(info->Hash) == true && characterStateFlags.IsPetNotUsable == false)
		{
			UnsummonPet();			if(inventory.IsAbleToUse(info, gameType, this, stageData->GetStageInfo()) == false)
				return ItemUseResult::Failure;

			if(inventory.UseItem(info, gameType, this, stageData->GetStageInfo()) != ItemUseResult::Success)
				return ItemUseResult::Failure;			
			
			std::vector<Database::Info::ActionInfo::State>::const_iterator itr = std::find_if(info->States.begin(), info->States.end(), StateTypeFinder(Database::Info::StateInfo::NOLOGIC));

			if(itr != info->States.end())
			{
				//StateList�� ���鼭 ����� ����.
				for(std::vector<Database::Info::ActionInfo::State>::const_iterator i = itr->optionalStates.begin(); i != itr->optionalStates.end(); ++i)
				{
					std::map<String, String> state = i->params;
					AddState(Stat::CreateState(i->type, this, &state, NULL));
				}

				return ItemUseResult::Success;
			}
			
		}

		return inventory.UseItem(info, gameType, this, stageData->GetStageInfo());
	}

	void Player::UsePetItem( const GlobalSerial& itemSerial )
	{
		PetsIter itrPet = std::find_if(myPets.begin(), myPets.end(), PetWithSummonInfoFinder::ByPetSerial(itemSerial));

		if(itrPet != myPets.end())
		{
			itrPet->Pet.Appear = true;
			itrPet->Pet.EffectStateDelayTime = 0.0f;

			stageData->PetInfoUpdated( this, itrPet->Pet, itrPet->PetNPCSerial, L"" );
		}
	}

	void Player::DropPetItem(const GlobalSerial& itemSerial)
	{
		PetsIter itr = std::find_if(myPets.begin(), myPets.end(), PetWithSummonInfoFinder::ByPetSerial(itemSerial));

		if( itr != myPets.end() )
		{
			if( itr->Pet.Appear == true )
			{
				PetUnsummon( itr->Pet );
				stageData->PetDeleted( this, itemSerial );
			}

			myPets.erase(itr);
		}
	}

	bool Player::Equip(const Database::Info::ItemInfo* info, int64 fields, Constants::Equipment where, uint32& equipmentOld, bool bForce)
	{
		bool returnValue = inventory.Equip(info, fields, where, equipmentOld, this, bForce, stageData->GetStageInfo()->RebirthCountEquipLimit);
		if ( returnValue ) {
			if (info)
				playerData.Equipments.at(where).Id = info->Hash;
			else
				playerData.Equipments.at(where).Id = 0; //��������
		}
		return returnValue;
	}

	bool Player::ChangeEquipToEquip(Constants::Equipment from,Constants::Equipment to)
	{
		if( inventory.ChangeEquipToEquip(from,to) == false ) {
			Logger::GetInstance().Error(L"Logic::Player::ChangeEquip Failed - not same equip part from : {0}, to : {1}",static_cast<int>(from),static_cast<int>(to));
			return false;
		}
		Lunia::XRated::Item itemTemp = playerData.Equipments.at(from);
		playerData.Equipments.at(from) = playerData.Equipments.at(to);
		playerData.Equipments.at(to) = itemTemp;
		return true;
	}

	bool Player::SwapEquipment( const std::vector< EquippedItem > & newEquipments )
	{
		uint32 equipmentOld;
		float hp = GetHP();
		
		for( size_t equipment=0; equipment<newEquipments.size(); ++equipment )
		{
			inventory.Unequip( newEquipments[equipment].where, equipmentOld, this );
			playerData.Equipments.at(newEquipments[equipment].where).Id = 0;
		}
		
		bool returnValue = true;
		for( size_t equipment=0; equipment<newEquipments.size(); ++equipment )
		{
			const Database::Info::ItemInfo* itemInfo = Database::DatabaseInstance().InfoCollections.Items.Retrieve( newEquipments[equipment].itemHash );			
			if( itemInfo ) 
			{
				returnValue &= inventory.Equip(itemInfo , newEquipments[equipment].InstanceEx, newEquipments[equipment].where, equipmentOld, this, true/*bforce*/, stageData->GetStageInfo()->RebirthCountEquipLimit);
			}
			if ( returnValue ) 
			{
				if (itemInfo)
				{
					playerData.Equipments.at(newEquipments[equipment].where).Id = newEquipments[equipment].itemHash;
				}
				else
				{
					playerData.Equipments.at(newEquipments[equipment].where).Id = 0;
				}
			}
		}
		SetHP( hp );
		return returnValue;
	}

	void Player::ValidateEquippedItems()
	{
        inventory.ValidateEquippedItems(this, stageData->GetStageInfo()->RebirthCountEquipLimit);
	}

	bool Player::IsEnableEquipment(Constants::Equipment where)
	{
        return inventory.IsEnableEquipment(where);
	}

	void Player::AddEquipSetBonus()
	{
        inventory.AddEquipSetBonus(this);
	}

	bool Player::Rebirth( uint32 levelAfterRebirth )
	{
		//���½��и� ����ؼ� �ѹ��س��´�.
		rebirthRollBackPlayerData = playerData;
		rebirthRollBackLevel = characterData.Level;
		rebirthRollBackXp = characterData.Xp;

		if( levelAfterRebirth < 1 || levelAfterRebirth > 99 )
		{
			levelAfterRebirth = 1;
		}

		playerData.StoredSkillPoint += DatabaseInstance().InfoCollections.Statuses.GetRebirthSkillPoint(characterData.Level);

		/// Skill Reset
		std::vector<uint32> skillGroupHash;

		for(std::vector<Skill>::iterator itr = playerData.Skills.begin(); itr != playerData.Skills.end(); ++itr)
			skillGroupHash.push_back(itr->Id);

		for(std::vector<uint32>::iterator itr = skillGroupHash.begin(); itr != skillGroupHash.end() ; ++itr)
			ResetSkillLevel(*itr);

		////Level 1�϶� ��ų����Ʈ�� �⺻������ ������ �Ǿ�� �ϱ� ������ ���½� ������ 1�� �����ش�. by hak
		//���� �� ������ 1�� �ƴ� �� �����Ƿ� level�� ���� ����ؾ� �Ѵ�.
		uint32 temp = levelAfterRebirth + static_cast< uint32 >( levelAfterRebirth / 4 );
		playerData.SkillPoint = playerData.AddedSkillPointPlus + playerData.StoredSkillPoint + temp; 

        //++playerData.StoredLevel;
		playerData.StoredLevel += levelAfterRebirth;
		++playerData.RebirthCount;
		playerData.LastRebirthDateTime = Lunia::DateTime::Now();
		characterData.Level = levelAfterRebirth;
		characterData.Xp = 0;

		/// Level Change
		int addStr = status.Str - playerStatus.info->Strength;
		int addDex = status.Dex - playerStatus.info->Dextrity;
		int addVit = status.Vit - playerStatus.info->Vitality;
		int addInt = status.Int - playerStatus.info->Intelligence;

		rebirthRollBackAddStr = addStr;
		rebirthRollBackAddDex = addDex;
		rebirthRollBackAddVit = addVit;
		rebirthRollBackAddInt = addInt;

		/// Status Calculate
		status.dmg.str = 0;
		status.dmg.dex = 0;
		LoadStatus();

		status.Str += addStr;
		status.Dex += addDex;
		status.Vit += addVit;
		status.Int += addInt;

		status.dmg.str = status.GetStr();
		status.dmg.dex = status.GetDex();

		RecalculateStatus();
		ResetAllFamiliarStatusByPlayerStat();

		return stageData->Rebirthed(*this);
	}

	void Player::RebirthRollBack()
	{
		playerData = rebirthRollBackPlayerData;
		characterData.Level = rebirthRollBackLevel;
		characterData.Xp = rebirthRollBackXp;

		for(std::vector<Skill>::iterator itr = playerData.Skills.begin(); itr != playerData.Skills.end(); ++itr)
			SetSkillLevel(itr->Id, itr->Level);
			//IncSkillLevel(itr->Id, itr->Level);

		/// Status Calculate
		status.dmg.str = 0;
		status.dmg.dex = 0;
		LoadStatus();

		status.Str += rebirthRollBackAddStr;
		status.Dex += rebirthRollBackAddDex;
		status.Vit += rebirthRollBackAddVit;
		status.Int += rebirthRollBackAddInt;

		status.dmg.str = status.GetStr();
		status.dmg.dex = status.GetDex();

		RecalculateStatus();
		ResetAllFamiliarStatusByPlayerStat();

		stageData->RebirthRollBacked(*this);
	}

	void Player::Die()
	{
		bDead = true;
		this->GetPlayerData().bDead = true;

		ghostRule = stageData->GetStage().GetGhostRule();

		switch ( ghostRule ) {
		case Constants::GhostRules::TombStone :
			{
				TombStone* tomb = LogicDBInstance().CreateTombstone( stageData, objectData );
				Lunia::XRated::Serial serial = stageData->GetNextObjectSerial();
				tomb->SetSerial( serial );
				stageData->AddToObjectBuffer(stageData->GetStageCnt(), tomb);
				serialTombStone = serial;
			}
			break;
		case Constants::GhostRules::InstantPop :
		case Constants::GhostRules::InstantPopAtWill :
		case Constants::GhostRules::AutoTimeRevive_Sec10 :
		case Constants::GhostRules::AutoTimeRevive_Sec5 :
		case Constants::GhostRules::AutoTimeRevive_Sec3 :
			tGhost = 0;
			break;
		}
		ResetCoolDown(objectData.GameObjectSerial); // ��ų ��ٿ��� �����ش�.
		ResetBundle(); // ����, ����, �� �� �����ش�.
		status.heightControl.speed = 0; //������ ���߿� ���ִ��� �ٷ� ���� �ٴ´�. ������ �ʱ�ȭ�� �ȵǼ� ����Ƣ�� ���׸� ����.

		NoticeDestroyToFamiliar(true);
		familiars.clear();

		//Air Combo Clear
		//((Actor*)this)->ClearAirAttackData();

		stageData->Died( this );
	}

	void Player::ResetBundle()
	{
		stateBundleManager.ClearBuff();
	}

	bool Player::DailyRevive(bool bSelf, float refillRate, bool bFromTombStone, uint64 orderNumber /* instantCoinRevive ������ ����. */)
	{
		std::cout << "Player::DailyRevive" << std::endl;
		///	���ľ��� ����� �� X, ķ�Ķ����� �ڿ� �̼� Ŭ���� üũ�ϸ� �ȵǞ��������� if (!bDead || stageData->GetStage().IsMissionCleared()) return;
		if (!bDead) return false; // arcrus. note. ��ú�Ȱ �Ҷ���, ���⼭ �ɸ��� false �� �Ǽ� �ѹ��� �Ҹ��ٵ�. ���������?

		switch ( ghostRule ) {
		case Constants::GhostRules::AutoTimeRevive_Sec10 :
		case Constants::GhostRules::AutoTimeRevive_Sec5 :
		case Constants::GhostRules::AutoTimeRevive_Sec3 :
			{
				if ( !bSelf ) //������ ��Ƴ��°Ծƴϴ�.. ��Ȱ�� �޾Ҵ���. ���������� ������
					break;

				if(ghostRule == Constants::GhostRules::AutoTimeRevive_Sec10)
				{
					if ( tGhost < Constants::GhostRules::tGhostWhenAutoTimeRevive_Sec10 )
						return false;
				}
				else if(ghostRule == Constants::GhostRules::AutoTimeRevive_Sec5)
				{
					if ( tGhost < Constants::GhostRules::tGhostWhenAutoTimeRevive_Sec5 )
						return false;
				}
				if(ghostRule == Constants::GhostRules::AutoTimeRevive_Sec3)
				{
					if ( tGhost < Constants::GhostRules::tGhostWhenAutoTimeRevive_Sec3 )
						return false;
				}
				if ( !playerData.Life )
					return false;
//				--playerData.Life;
//				--playerData.PS.Life;

				//StageInfo���� respawn point�� ���ͼ� ��ġ�� �Ű�����.
				float3 respawnPos;
				if ( stageData->GetRespawnPos(characterData.Team, respawnPos) )
					SetPosition(respawnPos);
			}
			break;
		case Constants::GhostRules::InstantPopAtWill :
			{
				if ( !bSelf ) //������ ��Ƴ��°Ծƴϴ�.. ��Ȱ�� �޾Ҵ���. ���������� ������
					break;

				if ( !playerData.Life )
					return false;
				--playerData.Life;
				--playerData.UsableBonusLifeInStage;
                //--playerData.PS.Life;

				//StageInfo���� respawn point�� ���ͼ� ��ġ�� �Ű�����.
				float3 respawnPos;
				if ( stageData->GetRespawnPos(characterData.Team, respawnPos) )
					SetPosition(respawnPos);
			}
			break;
		case Constants::GhostRules::InstantPop :
			{
				if ( !bSelf ) //������ ��Ƴ��°Ծƴϴ�.. ��Ȱ�� �޾Ҵ���. ���������� ������
					break;

				if ( tGhost < Constants::GhostRules::tGhostWhenInstantPop )
					return false;
				if ( !playerData.Life )
					return false;
				--playerData.Life;
				--playerData.UsableBonusLifeInStage;

				//--playerData.PS.Life;
			}
			break;
		case Constants::GhostRules::TombStone :
			{
				TombStone* tombstone = (TombStone*)(stageData->Retrieve(serialTombStone));
				//������ ���� ���¿����� ���� �� �� �ְα�.
				if (!tombstone) {
					//Lunia_INFO(( L"[Player::Revive] unable to find tomestone [%s / {}]", GetName().c_str(), serialTombStone));
					return false;
				}

				if (bSelf) {
					//������ ���� ����ġ����, �ƴϸ� ������ ������ �ʴ�.
					float dist = Lunia::Math::Length<float>(objectData.Position - tombstone->GetPosition());
					if ( playerData.UsableBonusLifeInStage <= 0 || dist > TombStone::ReviveRange )
						return false;
					--playerData.UsableBonusLifeInStage;
					//--playerData.PS.Life;
				}

				if (bFromTombStone)
					objectData.Position = tombstone->GetPosition();

				stageData->ObjectDestroyed(tombstone, Constants::ObjectType::TombStone/*, objectData.Position, tombstone->GetDirection(), 0, 0*/);
				if (tombstone->GetType()!=XRated::Constants::TombStone)
					Logger::GetInstance().Exception(L"Not a tombstone!!!");
				if ( stageData->Remove(serialTombStone) )
					delete tombstone;
			}
			break;
		}

		bDead = false;
		this->GetPlayerData().bDead = false;
		lastHittedPlayer = 0;

		RemoveState(stateBuffers.state_defence, Info::StateInfo::Type::GHOST);
		SetAction( Info::HashTable::Actions::Stand, NULL, true);

		std::vector<Actor*> l;
		stageData->GetAreaObject( GetPosition(), GetRadius(), Constants::AllianceType::AnyObjects, 0, this, l);
		for ( std::vector<Actor*>::iterator i = l.begin() ; i != l.end() ; ++i )
			if ( (*i)->GetType() == Constants::ObjectType::NonPlayer )
				(*i)->SetOverlap(true);
		SetOverlap(true); //�ٸ� ���� ���� ���� �� �ִ�.

		SetHP(status.MaxHp * refillRate);
		SetMP(status.MaxMp * refillRate);

		stageData->Revived(this, bSelf);

		// arcrus. KR#5711. ��Ȱ���� ����Ͽ� ��Ȱ�� �ϰ� �Ǹ�, �� ��Ȱ�� �ȵǴ� ���� ����.
		// ���⼭ ���� �ʿ䰡 ��� �ּ� ó��
		//if ( petUnsummonedAfterDie )
		//{
		//	petUnsummonedAfterDie = false;
		//	stageData->PetRevived( this );
		//}

				
		return true;
	}
	bool Player::Revive( const ReviveStatus::Type reviveStatus, bool bSelf, float refillRate, bool bFromTombStone, uint64 orderNumber /* instantCoinRevive ������ ����. */)
	{
		std::cout << "Player::Revive" << std::endl;
		///	���ľ��� ����� �� X, ķ�Ķ����� �ڿ� �̼� Ŭ���� üũ�ϸ� �ȵǞ��������� if (!bDead || stageData->GetStage().IsMissionCleared()) return;
		if (!bDead) return false; // arcrus. note. ��ú�Ȱ �Ҷ���, ���⼭ �ɸ��� false �� �Ǽ� �ѹ��� �Ҹ��ٵ�. ���������?

		

		// �ѹ�˻�����. ��ú�Ȱ�϶���! --> arcrus. note. �˻��� �ʿ伺�� ���� �켱�� �ּ� ó���Ѵ�.
		if ( orderNumber != 0 ) // orderNumber �� �ݵ�� 0 �� �ƴϴ�.
		{
			Logger::GetInstance().Warn( L"Logic Player::Revive by InstantCoinRevive - userNumber : {0}", orderNumber );
			//if ( Lunia::XRated::Service::Stage::CoinItemPurchaseManagerInstance().
			//	CheckRollbackPurchaseCoinItem( orderNumber ) == true )
			//{
			//	// ���������� �ѹ���.
			//	return false;
			//}
		}

		switch ( ghostRule ) {
		case Constants::GhostRules::AutoTimeRevive_Sec10 :
		case Constants::GhostRules::AutoTimeRevive_Sec5 :
		case Constants::GhostRules::AutoTimeRevive_Sec3 :
			{
				if ( !bSelf ) //������ ��Ƴ��°Ծƴϴ�.. ��Ȱ�� �޾Ҵ���. ���������� ������
					break;

				if(ghostRule == Constants::GhostRules::AutoTimeRevive_Sec10)
				{
					if ( tGhost < Constants::GhostRules::tGhostWhenAutoTimeRevive_Sec10 )
						return false;
				}
				else if(ghostRule == Constants::GhostRules::AutoTimeRevive_Sec5)
				{
					if ( tGhost < Constants::GhostRules::tGhostWhenAutoTimeRevive_Sec5 )
						return false;
				}
				if(ghostRule == Constants::GhostRules::AutoTimeRevive_Sec3)
				{
					if ( tGhost < Constants::GhostRules::tGhostWhenAutoTimeRevive_Sec3 )
						return false;
				}
				if ( !playerData.Life )
					return false;
//				--playerData.Life;
//				--playerData.PS.Life;

				//StageInfo���� respawn point�� ���ͼ� ��ġ�� �Ű�����.
				float3 respawnPos;
				if ( stageData->GetRespawnPos(characterData.Team, respawnPos) )
					SetPosition(respawnPos);
			}
			break;
		case Constants::GhostRules::InstantPopAtWill :
			{
				if ( !bSelf ) //������ ��Ƴ��°Ծƴϴ�.. ��Ȱ�� �޾Ҵ���. ���������� ������
					break;

				if ( !playerData.Life )
					return false;
				--playerData.Life;
				//--playerData.PS.Life;

				//StageInfo���� respawn point�� ���ͼ� ��ġ�� �Ű�����.
				float3 respawnPos;
				if ( stageData->GetRespawnPos(characterData.Team, respawnPos) )
					SetPosition(respawnPos);
			}
			break;
		case Constants::GhostRules::InstantPop :
			{
				if ( !bSelf ) //������ ��Ƴ��°Ծƴϴ�.. ��Ȱ�� �޾Ҵ���. ���������� ������
					break;

				if ( tGhost < Constants::GhostRules::tGhostWhenInstantPop )
					return false;
				if ( !playerData.Life )
					return false;
				--playerData.Life;
				//--playerData.PS.Life;
			}
			break;
		case Constants::GhostRules::TombStone :
			{
				TombStone* tombstone = (TombStone*)(stageData->Retrieve(serialTombStone));
				//������ ���� ���¿����� ���� �� �� �ְα�.
				if (!tombstone) {
					//Lunia_INFO(( L"[Player::Revive] unable to find tomestone [%s / {}]", GetName().c_str(), serialTombStone));
					return false;
				}

				if (bSelf) {
					//������ ���� ����ġ����, �ƴϸ� ������ ������ �ʴ�.
					float dist = Lunia::Math::Length<float>(objectData.Position - tombstone->GetPosition());
					if ( playerData.Life <= 0 || dist > TombStone::ReviveRange )
						return false;
					--playerData.Life;
					//--playerData.PS.Life;
				}

				if (bFromTombStone)
					objectData.Position = tombstone->GetPosition();

				stageData->ObjectDestroyed(tombstone, Constants::ObjectType::TombStone/*, objectData.Position, tombstone->GetDirection(), 0, 0*/);
				if (tombstone->GetType()!=XRated::Constants::TombStone) 
					Logger::GetInstance().Exception(L"Not a tombstone!!!");
				if ( stageData->Remove(serialTombStone) )
					delete tombstone;
			}
			break;
		}

		bDead = false;
		this->GetPlayerData().bDead = false;
		lastHittedPlayer = 0;

		RemoveState(stateBuffers.state_defence, Info::StateInfo::Type::GHOST);
		SetAction( Info::HashTable::Actions::Stand, NULL, true);
		//���⿡ ����ġ�� ���� �۾��ϸ�ȴ�. ��Ȱ�׼�
		if ( stageData->GetStageGroupInfo()->GameType & XRated::Constants::GameTypes::PvpGameTypeMask )
		{
			if( characterStateFlags.IsInvincibleAfterRevive == 1 )
			{
				SetAction( Info::HashTable::Actions::PvpRevive, NULL, true);
				Logger::GetInstance().Warn( L"ReviveStatus::Pvp" );
			}
			else
			{
				SetAction( Info::HashTable::Actions::Stand, NULL, true);
				Logger::GetInstance().Warn( L"ReviveStatus::None" );
			}
		}
		else
		{
		switch ( reviveStatus )
		{
			case ReviveStatus::None:
				SetAction( Info::HashTable::Actions::Stand, NULL, true);
				Logger::GetInstance().Warn( L"ReviveStatus::None" );
			case ReviveStatus::Basic:
				SetAction( Info::HashTable::Actions::BasicRevive, NULL, true);
				Logger::GetInstance().Warn( L"ReviveStatus::Basic" );
				break;
			case ReviveStatus::Stone:
				SetAction( Info::HashTable::Actions::StoneRevive, NULL, true);
				Logger::GetInstance().Warn( L"ReviveStatus::Stone" );
				break;
			case ReviveStatus::Instance:
				SetAction( Info::HashTable::Actions::InstanceRevive, NULL, true);
				Logger::GetInstance().Warn( L"ReviveStatus::Instance" );
				break;
			case ReviveStatus::Skill:
				SetAction( Info::HashTable::Actions::SkillRevive, NULL, true);
				Logger::GetInstance().Warn( L"ReviveStatus::Skill" );
				break;
			default:
				SetAction( Info::HashTable::Actions::Stand, NULL, true);
				Logger::GetInstance().Error( L"Wrong ReviveStatus" );
				break;
			};
		}

		std::vector<Actor*> l;
		stageData->GetAreaObject( GetPosition(), GetRadius(), Constants::AllianceType::AnyObjects, 0, this, l);
		for ( std::vector<Actor*>::iterator i = l.begin() ; i != l.end() ; ++i )
			if ( (*i)->GetType() == Constants::ObjectType::NonPlayer )
				(*i)->SetOverlap(true);
		SetOverlap(true);

		SetHP(status.MaxHp * refillRate);
		SetMP(status.MaxMp * refillRate);

		UpdatePvpRankRwward();

		stageData->Revived(this, bSelf);


		return true;
	}

	bool Player::Command(Action::Action::CommandType state, uint32 param)
	{
		if ( CheckState(Database::Info::StateInfo::Type::REVERSECMD) )
			param = PhysicalFunctions::ReverseCmd(param);

		return Actor::Command(state, param);
	}

	bool Player::DoDamage(Object* who, uint32 creatorActionHash, float minDmg, float maxDmg, float damage, Constants::DamageType dmgType, uint32 stateID, Constants::AttackStateType attackType, unsigned int& sFlag)
	{
		if(stageData->GetStageGroupInfo()->GameType & XRated::Constants::GameTypes::PvpGameTypeMask)// && CheckState(Database::Info::StateInfo::FATAL))
		{
			Serial	hitPlayerSerial = 0;

			if(who != NULL)
			{
				switch(who->GetType())
				{
				case	XRated::Constants::ObjectType::Player:
					hitPlayerSerial = who->GetSerial();
					break;
				case	XRated::Constants::ObjectType::Projectile:
					hitPlayerSerial = static_cast<XRated::Logic::Projectile*>(who)->GetCreatorSerial();
					break;			
				case	XRated::Constants::ObjectType::NonPlayer:
					hitPlayerSerial = static_cast<XRated::Logic::NonPlayer*>(who)->GetOwnerSerial();
					break;
				}
			}

			lastHittedPlayer = hitPlayerSerial;
		}

		return Actor::DoDamage(who, creatorActionHash, minDmg, maxDmg, damage, dmgType, stateID, attackType,sFlag);
	}

	bool Player::Update(float dt, IGameStateDatabase* db)
	{
		if ( bDead && playerData.Life) {
			switch( ghostRule ) {
			case Constants::GhostRules::Type::AutoTimeRevive_Sec10 :
			case Constants::GhostRules::Type::AutoTimeRevive_Sec5 :
			case Constants::GhostRules::Type::AutoTimeRevive_Sec3 :
				tGhost += dt;
				if( ghostRule == Constants::GhostRules::Type::AutoTimeRevive_Sec10 )
				{
					if ( tGhost > Constants::GhostRules::tGhostWhenAutoTimeRevive_Sec10 ) {
						float3 respawnPos;
						if ( stageData->GetRespawnPos(characterData.Team, respawnPos) )
							SetPosition(respawnPos);
						Revive( ReviveStatus::None, true);
					}
				}
				else if( ghostRule == Constants::GhostRules::Type::AutoTimeRevive_Sec5 )
				{
					if ( tGhost > Constants::GhostRules::tGhostWhenAutoTimeRevive_Sec5 ) {
						//StageInfo���� respawn point�� ���ͼ� ��ġ�� �Ű�����.
						float3 respawnPos;
						if ( stageData->GetRespawnPos(characterData.Team, respawnPos) )
							SetPosition(respawnPos);
						Revive( ReviveStatus::None, true);
					}
				}
				else if( ghostRule == Constants::GhostRules::Type::AutoTimeRevive_Sec3 )
				{
					if ( tGhost > Constants::GhostRules::tGhostWhenAutoTimeRevive_Sec3 ) {
						//StageInfo���� respawn point�� ���ͼ� ��ġ�� �Ű�����.
						float3 respawnPos;
						if ( stageData->GetRespawnPos(characterData.Team, respawnPos) )
							SetPosition(respawnPos);
						Revive( ReviveStatus::None, true);
					}
				}
				break;
			case Constants::GhostRules::Type::InstantPop :
				tGhost += dt;
				//3�ʰ� ������ �������.
				if ( tGhost > Constants::GhostRules::tGhostWhenInstantPop )
					Revive( ReviveStatus::None, true);
				break;
			case Constants::GhostRules::Type::InstantPopAtWill :
				tGhost += dt;
				//5�ʰ� ������ �������.
				if ( tGhost > Constants::GhostRules::tGhostWhenInstantPop ) {
					//StageInfo���� respawn point�� ���ͼ� ��ġ�� �Ű�����.
					float3 respawnPos;
					if ( stageData->GetRespawnPos(characterData.Team, respawnPos) )
						SetPosition(respawnPos);
					Revive( ReviveStatus::None, true);
				}
				break;
			}
		} else if ( CheckState(Database::Info::StateInfo::Type::MORPH) ) {
			//TODO : ���̻����� �� �Ź� ȣ�����ִ� ���� ��ȿ��. ���� �����غ���. by jiff100
			autoActionManager.Init(); //���εƴٸ� ���̻� �ڵ��׼��� �̾����� �ʴ´�.

			if ( GetHP() <= 0 ) 
			{
				if (morphStateBundleInfo) 
				{
					stateBundleManager.Remove(morphStateBundleInfo);
					morphStateBundleInfo = NULL;

					if (lastActionInfo.bExist)
					{
						SetAction(lastActionInfo.actionName, lastActionInfo.actionParam, lastActionInfo.bForce);
						lastActionInfo.Clear();
					}
					else
						SetAction(Database::Info::HashTable::Actions::Stand, NULL, true);
				}
				else
				{
					ResetBundle();
					SetAction(Database::Info::HashTable::Actions::GhostRise, NULL, true);
				}

			} else 
			{
				inventory.Update(dt);
				skillManager.CoolDown(dt);
			}
		} else {
			inventory.Update(dt);
			skillManager.CoolDown(dt);
		}

		// ��ȯ�Ǿ� �ִ� Pet ����ġ ������Ʈ : Pet ���� ���Ŀ� Player ������ �Űܼ� dt�� �Բ� ���
		PetUpdate(dt);

		UpdateAutoLinkAction(dt);

		stylePoint.Process(dt); /* StylePoint */
		return Actor::Update(dt, db);
	}

	bool Player::SetAction(uint32 name, Action::Action::IActionParam* param, bool force) 
	{
		if ( CheckState(Database::Info::StateInfo::Type::MORPH) ) 
		{		
			lastActionInfo.Clear();

			lastActionInfo.bExist = true;
			lastActionInfo.actionName = name;
			lastActionInfo.actionParam = NULL;
			if ( param )
			{
				lastActionInfo.actionParam = param->GetClonePtr();
			}
			lastActionInfo.bForce = force;
		}

		return Actor::SetAction(name, param, force);
	}

	void Player::XpGained(XRated::Constants::ExpAcquiredType type, uint64 xp, bool bUsingExpFactor, bool bUsingWeightValue, Serial beKilledNpc)
	{
		if ( GetPlayTimePenalty() & XRated::Constants::PlayTimePenalty::Exp ) { //�����ߵ������ý��� ������
			return;
		}
		if ( characterData.Level >= Constants::PlayerMaxLevel )
			return;

		if ( bUsingExpFactor ) {
			xp = (uint64)((float)xp * playerData.expFactor);
		}

		//���� ����ġ �ǳʽ��� �ش�
		if (GetLevel() >= 70)
			xp = (uint64)((float)xp * ExpFunctions::Stage::GetAddExpFactorByRebirthCount(GetRebirthCount() ) );
		

		//if(isGainNormal == false)
		//{
		//	switch(type)
		//	{
		//	case	XRated::Constants::ExpScript:
		//		xp = 0;		//	Secret Found �� ��ũ��Ʈ���� �ִ� ��� ����ġ�� ���ش�.
		//		break;
		//	case	XRated::Constants::ExpNonPlayer:
		//		xp = (int32)((float)xp * 0.2);	// 20%�� ����ġ�� ��´�
		//		break;
		//	case	XRated::Constants::ExpRank:
		//		xp = 0;		//	�������� Ŭ���� ����ġ�� ����. ���� ĳ�Ƚ�
		//		break;
		//	default:
		//		break;
		//	}
		//}
			switch(type)
			{
			case	XRated::Constants::ExpScript:
			case	XRated::Constants::ExpNonPlayer:
			case	XRated::Constants::ExpRank:
				{
					int stageProprietyLevel = stageData->GetStageProprietyLevel();
					if ( stageProprietyLevel > GetLevel() )
					{
						int levelGap = stageProprietyLevel - GetLevel();
						if ( levelGap >= 1 && levelGap <= 10)
						{
							xp = static_cast<uint64>( static_cast<float>(xp) * ((101.f - static_cast<float>(levelGap) ) / 100.f) );
						}
						else if ( levelGap >= 11 && levelGap <= 15)
						{
							xp = static_cast<uint64>( static_cast<float>(xp) * ((200.f - (static_cast<float>(levelGap)*10) ) / 100.f) );
						}
						else if ( levelGap >= 16 && levelGap <= 55)
						{
							xp = static_cast<uint64>( static_cast<float>(xp) * ((65.f - static_cast<float>(levelGap) ) / 100.f) );
						}
						else if ( levelGap > 55 )
						{
							xp = static_cast<uint64>( static_cast<float>(xp) * 0.1f);
						}
						//else if ( levelGap >= 11 && levelGap <= 30)
						//{
						//	xp = static_cast<int32>( static_cast<float>(xp) * ((110.f - static_cast<float>(levelGap) ) / 100.f) );
						//}
						//else if ( levelGap >= 31 && levelGap <= 50)
						//{
						//	xp = static_cast<int32>( static_cast<float>(xp) * ((140.f - static_cast<float>(levelGap * 2.f) ) / 100.f) );
						//}
						//else if ( levelGap >= 51 && levelGap <= 60)
						//{
						//	xp = static_cast<int32>( static_cast<float>(xp) * ((190.f - static_cast<float>(levelGap * 3.f) ) / 100.f) );
						//}
						//else if ( levelGap >= 61 && levelGap <= 99)
						//{
						//	xp = static_cast<int32>( static_cast<float>(xp) * 0.1f );	// 10%�� ����ġ�� ��´�
						//}
						//else if ( levelGap > 99 )
						//{
						//	xp = 0;
						//}
					}
				}
				break;
			default:
				break;
			}
		
		if (bUsingWeightValue) {
			xp = (uint64)((float)xp * ExpFunctions::Stage::GetWeightValue(GetLevel() ) ); //���� ����ġ�� ��������
		}

		///	�������� Ŭ���� ����ġ�� �����ѵ�
		if(type == XRated::Constants::ExpRank)
            clearXp += xp;
		/// ����Ʈ�� ������ ��� ����ġ�� �����Ѵ�.
		else if(type != XRated::Constants::ExpQuest)
			stageXp += xp;

		stageData->XpGained(this, type, (uint64)xp, 0, 0, beKilledNpc);

		characterData.Xp += xp;
		//playerData.PS.Xp += xp;//��� display��
		uint32 neededXp = Database::DatabaseInstance().InfoCollections.Statuses.GetExp( Info::StatusInfoManager::ExpType::Story, characterData.Level-1);
		if (characterData.Xp >= neededXp) 
		{
            characterData.Xp -= neededXp;
			LevelUp();
			//SetHeight(0);
			if ( CheckState(Info::StateInfo::Type::GHOST) ) {
				SetAction( Info::HashTable::Actions::GhostWinPose, NULL, true);
			} else {
				SetAction( Info::HashTable::Actions::WinPose, NULL, true);

				//Familiar�鿡�� ������ �˸�
				//for (std::list<Familiar>::iterator i =  familiars.begin() ; i != familiars.end() ; ++i )
				//	(*i).object->MasterCommand(Database::Info::HashTable::Actions::WinPose, true);
			}

			float weightFactor = ExpFunctions::Stage::GetWeightValueFactor(characterData.Level);
		}
	}

	uint16 Player::PvpLevelUp(XRated::Constants::ExpAcquiredType type, int xp, uint32 xpPvp)
	{
		if ( GetPlayTimePenalty() & XRated::Constants::PlayTimePenalty::Exp ) { //�����ߵ������ý��� ������
			return playerData.PvpLevel;
		}

		if ( xp > 0 )
			xp = (int32)((float)xp * playerData.expFactor);

		//playerData.PS.Xp = xp;

		stageData->XpGained(this, type, 0, xp);

		if ( xp >= 0 ) {
			playerData.PvpXp += xp;
			if ( playerData.PvpXp >= xpPvp ) {
				playerData.PvpXp -= xpPvp;
				++playerData.PvpLevel;
				stageData->LevelUp(this, XRated::Constants::ExpType::Pvp, playerData.PvpLevel, playerData.StoredLevel);
			}
		} else {
			// �ϴ� �����ٿ��� �Ͼ�� �ʰ� ���´�.
			int currentXp = (int)playerData.PvpXp + xp;
			if ( currentXp < 0 )
				playerData.PvpXp = 0;
			else
				playerData.PvpXp = (uint32)currentXp;

			

			//bool bLevelDown = false;
			//xp = (int)playerData.PvpXp + xp;
			//while (xp < 0) {
			//	if ( playerData.PvpLevel == 1 ) { //Level 1 ���̻� ������ ���� ����.
			//		playerData.PS.Xp = -((int)playerData.PvpXp);
			//		playerData.PvpXp = 0;
			//		stageData->LevelUp(this, false, 1);
			//		return playerData.PvpLevel;
			//	} else {
			//		--playerData.PvpLevel;
			//		bLevelDown = true;
			//	}
			//	playerData.PvpXp = DatabaseInstance().InfoCollections.Statuses.GetExp(Info::StatusInfoManager::ExpType::Pvp, playerData.PvpLevel-1);
			//	xp = (int)playerData.PvpXp + xp;
			//}
			//if ( bLevelDown )
			//	stageData->LevelUp(this, false, playerData.PvpLevel);
		}

		ValidateEquippedItems();

		return playerData.PvpLevel;
	}

	uint16 Player::WarLevelUp(XRated::Constants::ExpAcquiredType type, int xp, uint32 xpWar)
	{
		if ( xp > 0 )
			xp = (int32)((float)xp * playerData.expFactor);

		stageData->XpGained(this, type, 0, 0, xp);

		if ( xp >= 0 ) {
			playerData.WarXp += xp;
			if ( playerData.WarXp >= xpWar ) {
				playerData.WarXp -= xpWar;
				++playerData.WarLevel;
				stageData->LevelUp(this, XRated::Constants::ExpType::War, playerData.WarLevel, playerData.StoredLevel);
			}
		} else {
			// �ϴ� �����ٿ��� �Ͼ�� �ʰ� ���´�.
			int currentXp = (int)playerData.WarXp + xp;
			if ( currentXp < 0 )
				playerData.WarXp = 0;
			else
				playerData.WarXp = (uint32)currentXp;

		}

		ValidateEquippedItems();

		return playerData.WarLevel;
	}

	uint16 Player::LevelUp()
	{
#ifdef _SINGLE
		if ( characterData.Level >= 80 )
			return 80;
#endif
		++characterData.Level;
		++playerData.StoredLevel;

		playerData.SkillPoint += ( characterData.Level % 4 ) ? 1 : 2;

		//status�� �������ش�.
		//�⺻���ȵ��� ���� ����
		//RemoveDamage(playerStatus.info->MinDmg, playerStatus.info->MaxDmg);
		status.Str -= playerStatus.info->Strength;
		status.Dex -= playerStatus.info->Dextrity;
		status.Int -= playerStatus.info->Intelligence;
		status.Vit -= playerStatus.info->Vitality;
		status.dmg.str = status.GetStr();
		status.dmg.dex = status.GetDex();
		//���� ������ �⺻ ������ ���Ѵ�.
		playerStatus.info = DatabaseInstance().InfoCollections.Statuses.Retrieve(characterData.Level, Constants::GetClassStr(playerData.Job));
		//AddDamage(playerStatus.info->MinDmg, playerStatus.info->MaxDmg);

		uint32 increaseStatusFactor = DatabaseInstance().InfoCollections.Statuses.GetRebirthAdditionalFactor(characterData.Level, playerData.RebirthCount);
		uint32 increaseMajorStatusFactor = DatabaseInstance().InfoCollections.Statuses.GetRebirthAdditionalMajorFactor(characterData.Level, playerData.RebirthCount);

		status.Str += playerStatus.info->Strength + increaseStatusFactor;
		status.Dex += playerStatus.info->Dextrity + increaseStatusFactor;
		status.Int += playerStatus.info->Intelligence + increaseStatusFactor;
		status.Vit += playerStatus.info->Vitality + increaseStatusFactor;

		switch (DatabaseInstance().InfoCollections.Statuses.GetMajorStatType(Constants::GetClassStr(playerData.Job)) )
		{
		case Constants::CharacterMajorStatType::Dex:
			status.Dex += (increaseMajorStatusFactor - increaseStatusFactor);
			break;
		case Constants::CharacterMajorStatType::Int:
			status.Int += (increaseMajorStatusFactor - increaseStatusFactor);
			break;
		case Constants::CharacterMajorStatType::Str:
			status.Str += (increaseMajorStatusFactor - increaseStatusFactor);
			break;
		case Constants::CharacterMajorStatType::Vit:
			status.Vit += (increaseMajorStatusFactor - increaseStatusFactor);
			break;
		}

		status.basicHp = playerStatus.info->Hp;
		status.basicMp = playerStatus.info->Mp;
		status.stackedInt = playerStatus.info->Intelligence;
		status.stackedVit = playerStatus.info->Vitality;

		status.dmg.str = status.GetStr();
		status.dmg.dex = status.GetDex();
		//������ �������� 2�� ������ ���ȵ��� �ٽ� ����Ѵ�.
		RecalculateStatus();
		ResetAllFamiliarStatusByPlayerStat();

		//Hp, Mp ä���ش�. ���� �����϶��� ����
		if ( !CheckState(Info::StateInfo::Type::GHOST) ) {
			status.Mp = status.MaxMp;
			status.Hp = status.MaxHp;
		}

		RemoveState(Info::StateInfo::Sort::ORDINARYSTATE, regenHPId);
		RemoveState(Info::StateInfo::Sort::ORDINARYSTATE, regenMPId);
		//���� regen rate�� hp�� 0�ϰ�� ���̻� regen���� �ʴ´�.
		regenHP = new Stat::RegenHp(this, Stat::RegenHp::ChargeType::AMOUNT, playerStatus.regenHp, false);
		AddState(regenHP);
		regenHPId = regenHP->GetId();
		regenMP = new Stat::RegenMp(this, Stat::RegenMp::ChargeType::AMOUNT, playerStatus.regenMp, false);
		AddState(regenMP);
		regenMPId = regenMP->GetId();

		ValidateEquippedItems();

		stageData->LevelUp(this, XRated::Constants::ExpType::Stage, characterData.Level, playerData.StoredLevel);
		stageData->SkillPointGot(this, ( characterData.Level % 4 ) ? 1 : 2);
		return characterData.Level;
	}

	bool Player::Tamed(Constants::Familiar::Type type, NonPlayer* object)
	{
		//���� ��ȯ���� �־ �������ȴ�.
		RemoveSameFamiliar(object, type);
		
		{ 
			//��ȯ���� �ִ� ���� �� �ִ� ��ġ�� �ʰ��ϸ� ó���� ��ȯ�� �༮����� �����ش�.
			int familiarCount = 0;
			std::list<Familiar>::iterator it = familiars.begin();
			std::list<Familiar>::iterator firstItem = familiars.end();

			while( it != familiars.end() )
			{
				if( it->type == type )	
				{
					++familiarCount;
					if( firstItem == familiars.end() )	firstItem = it;
				}
				++it;
			}

			if( familiarCount >= Constants::Familiar::MaxCountForEachType[ type ] )
			{
				if(type == Constants::Familiar::Type::Pet)
					PetUnsummon(firstItem->object);
				else
				{
					firstItem->object->MasterCommandMeSuicide(true);
					familiars.erase( firstItem );
				}
			}	
		}

		if(type == Constants::Familiar::Type::Pet && PetSummon(object) == false)
		{
			Logger::GetInstance().Warn(L"Failed Summon Pet - {0}(player) is not owner", GetName().c_str());
			return false;
		}

		if(stageData->Tamed(this, object->GetSerial(), type) == false)
		{
			Logger::GetInstance().Warn(L"Failed Tame Pet - {0}(player) is not owner", GetName().c_str());
			return false;
		}

		familiars.push_back(Familiar(type, object));

		PetsIter itr = std::find_if(myPets.begin(), myPets.end(), PetWithSummonInfoFinder::ByPetNPCSerial(object->GetSerial()));

		if(itr != myPets.end())
			stageData->PetInfoUpdated(this, itr->Pet, object->GetSerial(), L"PET_SUMMON");

		if (type != Constants::Familiar::Type::Pet)
		{
			object->ResetFamiliarStatusByPlayerStat(status.GetStr(), status.GetDex(), status.GetInt(), status.GetVit(), stageData->GetStageInfo()->RebirthCountEquipLimit, true);
			object->AddDamage(GetPlusMinDamage(), GetPlusMaxDamage());
			//Lunia_IMPORTANT((L"Tamed Str:{}, Dex:{}, Int:{}, Vit:{}", status.Str, status.Dex, status.Int, status.Vit));
		}

		return true;
	}

	void Player::FamiliarDestroyed(NonPlayer* object)
	{
		//�÷��̾��� ��Ʈ�ѿ� ���ؼ� familiar�� �����Ǵ� ���̽����� familiars�� �����Ƿ� ó���� �� ����.
		//�÷��̾ �Ҹ��� ������ ���� �̹� ����Ʈ���� ������ �� �� �� �ִ�.
		for (std::list<Familiar>::iterator i =  familiars.begin() ; i != familiars.end() ; ++i ) {
			if ( (*i) == object ) {
				if( object->IsPetType() == true ) {
					PetUnsummon(object);
				}else {
					familiars.erase(i);
				}
				break;
			}
		}
	}

	void Player::ChangeFamiliarFormation(Constants::Familiar::Formation formation)
	{
		familiarFormation = formation;
		//int cnt=0;
		//for (std::list<Familiar>::iterator i =  familiars.begin() ; i != familiars.end() ; ++i, ++cnt )
		//	(*i).object->MasterCommandMeCome(GetPosition() + Constants::Familiar::FormationPosition[familiarFormation][cnt]);
	}
	

	void Player::NoticeDestroyToFamiliar(bool bSacrifice)
	{
		std::vector<NonPlayer*> petArray;
		
		for (std::list<Familiar>::iterator i =  familiars.begin() ; i != familiars.end() ; ++i )
		{
			if((*i).type != XRated::Constants::Familiar::Type::Pet)
				(*i).object->MasterDestroyed(bSacrifice);
			else
				petArray.push_back((*i).object);
		}

		if ( bSacrifice && petArray.size() > 0 ) // bSacrifice �÷��׸� �̷��� ����ϸ� �ȵǱ� ���ٵ� =��= ��.. �̰� �÷��̾ �׾������� üũ�ϴ� ����
		{
			petUnsummonedAfterDie = true;
		}

		for(std::vector<NonPlayer*>::iterator itr = petArray.begin(); itr != petArray.end(); ++itr)
			PetUnsummon(*itr);
	}

	void Player::ControlFamiliar(XRated::Serial targetSerial, uint16 targetIndex, Constants::Familiar::Command command)
	{
		if( targetSerial == 0 )
		{
			Logger::GetInstance().Error( L"Serial : {0} is not Familiar", targetSerial );
			return;
		}

		std::list<Familiar>::iterator i = familiars.begin();
		while( i != familiars.end() )
		{
			if( targetSerial == (*i).object->GetSerial() )
			{
				if( (*i).object->IsPetType() )
				{
					Logger::GetInstance().Error( L"Serial : {0} is not Familiar, but pet", targetSerial );
					return;
				}
				break;
			}
			++i;
		}
		
		if( i == familiars.end() )
		{
			Logger::GetInstance().Warn(L"The object which has serial {0} is not in familiars list", targetSerial);
			return;
		}

		switch(command) {
		case Constants::Familiar::Command::Attack :
			(*i).object->MasterCommandMeAttack(GetPosition() + GetDirection() * 70.0f);
			break;
		case Constants::Familiar::Command::Guard :
			(*i).object->MasterCommandMeGuard();
			break;
		case Constants::Familiar::Command::Come :
			if ( familiarFormation<Constants::Familiar::Formation::FormationCount && targetIndex<Constants::Familiar::MaxCount )
				(*i).object->MasterCommandMeCome(GetPosition() + Constants::Familiar::FormationPosition[familiarFormation][targetIndex]);
			else
				Logger::GetInstance().Error(L"invalid array range ({0}, {1})", (int)familiarFormation, targetIndex); 
			break;
		case Constants::Familiar::Command::Suicide :
			(*i).object->MasterCommandMeSuicide(true);
			break;
		case Constants::Familiar::Command::Praise :
			if ( SetAction(Database::Info::HashTable::Actions::Praise, NULL) )
				(*i).object->MasterPraiseMe(0.0f);
			break;
		}
	}

	float3 Player::GetFamiliarPosition(NonPlayer* object)
	{
		int cnt=0;
		float3 returnValue = GetPosition();
		returnValue.y = 0;
		for (std::list<Familiar>::iterator i =  familiars.begin() ; i != familiars.end() ; ++i,++cnt ) {
			if( i->object != NULL && i->object->IsPetType() )
			{
				--cnt;
			}

			if ( (*i) == object ) {
				if( object->IsPetType() )
				{
					if (familiarFormation<Constants::Familiar::Formation::FormationCount )
						returnValue += PhysicalFunctions::RotatePositionLimitedAngle(Constants::Familiar::FormationPosition[familiarFormation][Constants::Familiar::MaxCount - 1],
																							Constants::DirectionF::GetDirectionEnum(GetDirection()) );
				}
				else
				{
					if (familiarFormation<Constants::Familiar::Formation::FormationCount && cnt<Constants::Familiar::MaxCount)
						returnValue += PhysicalFunctions::RotatePositionLimitedAngle(Constants::Familiar::FormationPosition[familiarFormation][cnt],
																							Constants::DirectionF::GetDirectionEnum(GetDirection()) );					
					else
						Logger::GetInstance().Exception(L"invalid array range ({0}, {1})", (int)familiarFormation, cnt);
				}

				if (returnValue.x < 0 || returnValue.z < 0) {
					Logger::GetInstance().Warn("[Player::GetFamiliarPosition] Wrong familiar position. ({0},{1})({2},{3})[{4}/{5}]", returnValue.x, returnValue.z, GetPosition().x, GetPosition().z, stageData->GetStage().GetStageId(), (int)familiars.size());
					return float3(0,0,0);
				}
				return returnValue;
			}
		}
		returnValue += GetDirection() * -10.0f;
		if (returnValue.x < 0 || returnValue.z < 0) {
			Logger::GetInstance().Warn("[Player::GetFamiliarPosition] Wrong familiar position. ({0},{1})({2},{3})[{4}/{5}]", returnValue.x, returnValue.z, GetPosition().x, GetPosition().z, stageData->GetStage().GetStageId(), (int)familiars.size());
			return float3(0,0,0);
		}
		return returnValue;
	}

	void Player::ClearFamiliar()
	{
		std::vector< uint32 > appearPetsTemp;
		PetsIter iter = myPets.begin();
		while( iter != myPets.end() ) 
		{
			if( iter->PetNPCSerial != 0 )
			{
				appearPetsTemp.push_back(iter->PetNPCSerial);
			}

			++iter;
		}
		size_t max = appearPetsTemp.size();
		for( size_t i=0; i < max; ++i )
		{
			NonPlayer* npc = stageData->RetrieveNPC( appearPetsTemp.at(i) );
			if( npc != NULL ) {
				PetUnsummon( npc );
				//���� ���� ���� �Ѵ�.
				stageData->ObjectDestroyed(npc , npc->GetType(), true);				
			}else {
				Logger::GetInstance().Error( L"ClearFamiliar : can not foudn pet npc : {0}", appearPetsTemp.at(i));
			}
		}
		familiarFormation = Constants::Familiar::Formation::Follow;
		familiars.clear();
	}

	void Player::ResetAllFamiliarStatusByPlayerStat()
	{
		//Lunia_IMPORTANT((L"ResetAllFamiliarStatusByPlayerStat Str:{}, Dex:{}, Int:{}, Vit:{}", status.Str, status.Dex, status.Int, status.Vit));

		for (std::list<Familiar>::iterator i =  familiars.begin() ; i != familiars.end() ; ++i )
		{
			if((*i).type != XRated::Constants::Familiar::Type::Pet)
				(*i).object->ResetFamiliarStatusByPlayerStat(status.GetStr(), status.GetDex(), status.GetInt(), status.GetVit(), stageData->GetStageInfo()->RebirthCountEquipLimit);
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/* StylePoint Commands */
	void Player::ChangeStylePointState(XRated::StylePoint::State state)
	{
		switch (state)
		{
		case XRated::StylePoint::State::Start:
			stylePoint.Start(Lunia::XRated::Database::DatabaseInstance().InfoCollections.StylePoints.Retrieve().StylePointGaugeStartScore,
						 Lunia::XRated::Database::DatabaseInstance().InfoCollections.StylePoints.Retrieve().StylePointGaugeMaxScore);
			break;
		case XRated::StylePoint::State::Stop:
			stylePoint.Stop();
			break;
		case XRated::StylePoint::State::Restart:
			stylePoint.ReStart();
			break;
		}
	}

	void Player::ChangeStylePointPlayerCount(int count)
	{
		stylePoint.SetPlayerCount(count);
	}

	uint32 Player::GetStylePoint()
	{
		return static_cast<uint32>(stylePoint.GetCurrentStylePoint() );
	}

	float Player::GetSytlePointPercentage()
	{
		return	stylePoint.GetGaguePercentage();
	}

	void Player::AddStylePoint(const XRated::Constants::StylePointType& type, int count)
	{
		stylePoint.AddStylePoint(Lunia::XRated::Database::DatabaseInstance().InfoCollections.StylePoints.GetAddStylePoint(type, count));
	}

	void Player::AddAirComboStylePoint(int airComboCount)
	{
		if (airComboCount < XRated::Constants::LimitAirCombo) return;
		AddStylePoint(XRated::Constants::StylePointType::AirComboHit, (airComboCount - XRated::Constants::LimitAirCombo + 1) );
		//Lunia_IMPORTANT(("[Player::AddAirComboStylePoint]"));
	}

	void Player::AddFantasicSaveStylePoint()
	{
		if (static_cast<XRated::Constants::ClassType>(GetClass()) != XRated::Constants::ClassType::Healer) return;
	
		AddStylePoint(XRated::Constants::StylePointType::FantasticSave);
		//Lunia_IMPORTANT(("[Player::AddFantasicSaveStylePoint]"));
	}

	void Player::UpdateStylePoint()
	{
		AddBackStabStylePoint();
		AddDownAttackStylePoint();
		AddMultiHitStylePoint();
	}

	void Player::AddBackStabStylePoint()
	{
		if (static_cast<XRated::Constants::ClassType>(GetClass()) != XRated::Constants::ClassType::Thief) return;
		if (actions.GetAction(L"")->GetActionInfo()->animationName != L"Skill05") return;

		AddStylePoint(XRated::Constants::StylePointType::BackStabHit);
		//Lunia_IMPORTANT(("[Player::AddBackStabStylePoint]"));
	}

	void Player::AddDownAttackStylePoint()
	{
		if (static_cast<XRated::Constants::ClassType>(GetClass()) == XRated::Constants::ClassType::Slime ||
			static_cast<XRated::Constants::ClassType>(GetClass()) == XRated::Constants::ClassType::DarkTemplar ) return;
		if (actions.GetAction(L"")->GetActionInfo()->hash != Database::Info::HashTable::Actions::DownAtk) return;
		
		AddStylePoint(XRated::Constants::StylePointType::DownAttackHit);
		//Lunia_IMPORTANT(("[Player::AddDownAttackStylePoint]"));
	}

	void Player::AddMultiHitStylePoint()
	{
		if(multiHitCheckTimer + multiHitMaxTime < GetTickCount() || multiHitCheckTimer == 0)
		{
			multiHitCount = 0;
			multiHitCheckTimer = GetTickCount();
		}
		++multiHitCount;
		if( multiHitCount >= multiHitMaxCount )
		{
			multiHitCount = 0;
			multiHitCheckTimer = 0;

			AddStylePoint(XRated::Constants::StylePointType::MultiHit);
			//Lunia_IMPORTANT(("[Player::AddMultiHitStylePoint]"));
		}
	}

	void Player::SetEnableNextLicense(bool isEnableNextLicense)
	{
		this->isEnableNextLicense = isEnableNextLicense;
	}

	void Player::SetGainNormalExp(int minimumStageLevel)
	{
		if(isEnableNextLicense == false && minimumStageLevel != 0 && minimumStageLevel > GetLevel()+10)
		{
			isGainNormal = false;
			return;
		}

		isGainNormal = true;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void Player::AddSkillCooldownTime(uint32 skillGroupHash, float amount)
	{
		if(skillManager.GetSkillLevel(skillGroupHash) > 0)
			skillManager.AddSkillCooldownTime(skillGroupHash, amount);
	}

	bool Player::DoAutoLinkAction()
	{
		const Database::Info::ActionInfoManager::AutoAction& info = (*autoActionManager.info)[autoActionManager.currentStep];
		switch( info.command ) {
		case Constants::Command::Move :
			return Command(Action::Action::CommandType::MOVE, XRated::Constants::DirectionF::GetRealDirection(XRated::Constants::DirectionF::GetDirectionEnum(GetDirection()), (XRated::Constants::Direction)info.param));
		case Constants::Command::Dash :
			return Command(Action::Action::CommandType::DASH, XRated::Constants::DirectionF::GetRealDirection(XRated::Constants::DirectionF::GetDirectionEnum(GetDirection()), (XRated::Constants::Direction)info.param));
		case Constants::Command::Stop :
			return Command(Action::Action::CommandType::STOP, info.param);
		case Constants::Command::AttackA :
			return Command(Action::Action::CommandType::ATTACKA, XRated::Constants::DirectionF::GetRealDirection(XRated::Constants::DirectionF::GetDirectionEnum(GetDirection()), (XRated::Constants::Direction)info.param));
		case Constants::Command::AttackARelease :
			return Command(Action::Action::CommandType::ATTACKARELEASE, XRated::Constants::DirectionF::GetRealDirection(XRated::Constants::DirectionF::GetDirectionEnum(GetDirection()), (XRated::Constants::Direction)info.param));
		case Constants::Command::AttackB :
			return Command(Action::Action::CommandType::ATTACKB, XRated::Constants::DirectionF::GetRealDirection(XRated::Constants::DirectionF::GetDirectionEnum(GetDirection()), (XRated::Constants::Direction)info.param));
		case Constants::Command::AttackBRelease :
			return Command(Action::Action::CommandType::ATTACKBRELEASE, XRated::Constants::DirectionF::GetRealDirection(XRated::Constants::DirectionF::GetDirectionEnum(GetDirection()), (XRated::Constants::Direction)info.param));
		case Constants::Command::DownAttack :
			return Command(Action::Action::CommandType::DOWNATTACK, XRated::Constants::DirectionF::GetRealDirection(XRated::Constants::DirectionF::GetDirectionEnum(GetDirection()), (XRated::Constants::Direction)info.param));
		case Constants::Command::Jump :
			return Command(Action::Action::CommandType::JUMP, XRated::Constants::DirectionF::GetRealDirection(XRated::Constants::DirectionF::GetDirectionEnum(GetDirection()), (XRated::Constants::Direction)info.param));
		case Constants::Command::Cast :
			return UseSkill(info.param);
		}
		return false;
	}

	bool Player::AutoLinkAction(const Database::Info::ActionInfoManager::AutoActionList* actionList)
	{
		if ( actionList == NULL )
			return false;

		autoActionManager.Init();
		autoActionManager.info = actionList;
		if ( DoAutoLinkAction() )
			return true;
		return false;
	}

	void Player::UpdateAutoLinkAction(float dt)
	{
		if ( autoActionManager.IsPlaying() == false )
			return;

		autoActionManager.elapsedTime += dt;
		if ( autoActionManager.elapsedTime >= (*autoActionManager.info)[autoActionManager.currentStep].startTime ) {
			autoActionManager.elapsedTime -= (*autoActionManager.info)[autoActionManager.currentStep].startTime;
			if ( autoActionManager.info->size() > autoActionManager.currentStep+1 ) {//���� �ܰ谡 ���Ҵ�. ���� �ܰ��
				++autoActionManager.currentStep;
				if (DoAutoLinkAction() == false) {
					autoActionManager.Init();
				}
			} else {
				autoActionManager.Init();
			}
		}
	}

	//SetItem
	Player::SetItem* Player::GetSetItemInfo(std::wstring Id )
	{
		for ( std::list<Player::SetItem>::iterator i = setItems.begin(); i != setItems.end(); ++i )
		{
			if ( i->Id == Id )
				return &(*i);
		}
		return NULL;
	}
	void Player::AddSetItemInfo(SetItem setitem)
	{
		setItems.push_back(setitem);
	}

	bool Player::PetSummon(NonPlayer* object)
	{
		const Database::Info::Pet::Genus* genus = Database::DatabaseInstance().InfoCollections.Pets.GetGenus(object->GetNPCInfo()->HashcodeName);

		if(genus == NULL)
			Logger::GetInstance().Exception(L"can not found Pet::Genus : {0}", object->GetNPCInfo()->HashcodeName);

		size_t i;
		for(i = 0; i < myPets.size(); ++i)
		{
			/// ��...�̷� ������....
			//if(object->GetNPCInfo()->HashcodeName == myPets[i].PetHash)
			if(myPets[i].Pet.Appear == true)
			{
				myPets[i].Pet.ExpFactor = genus->ExpFactor;
				myPets[i].Pet.TiredFactor = genus->TiredFactor;
				break;
			}
		}

		if( i >= myPets.size() )
			return false;

		if( myPets[i].Pet.GetFeeling() == XRated::Pet::Feeling::State::Starving )
		{
			myPets[i].Pet.EmotionDelay = static_cast<float>(Lunia::Math::Random(30,60));
		}
		else
		{
			myPets[i].Pet.EmotionDelay = static_cast<float>(Lunia::Math::Random(30,180));
		}

		if(myPets[i].Pet.Level < Pet::MaxLevel) {
			myPets[i].Pet.NextLevelExp = Database::DatabaseInstance().InfoCollections.Statuses.GetExp( Database::Info::StatusInfoManager::ExpType::Pet, myPets[i].Pet.Level - 1 );
		}else {
			myPets[i].Pet.NextLevelExp = 0;
		}

		myPets[i].PetNPCSerial = object->GetSerial();
		AddPetEffectState(myPets[i].Pet, object);

		//���� Equipments�� �ִ� ��� ���� ó�� ����.
		const std::vector< XRated::Item > equipment( myPets[i].Pet.Equipments );				
		myPets[i].Pet.AllUnEquipment();
		std::vector< XRated::Item >::const_iterator iter = equipment.begin();
		std::vector< XRated::Item >::const_iterator end = equipment.end();
		while( iter != end ) {
			if( iter->Id != 0 ) {
				const Database::Info::ItemInfo* itemInfo = Database::DatabaseInstance().InfoCollections.Items.Retrieve( iter->Id );		
				uint32 oldTemp;
				XRated::Serial petObjectSerial;
				PetEquip( myPets[i].Pet.PetSerial, itemInfo, iter->InstanceEx, XRated::Constants::GetEquipPosition(itemInfo->EquipParts), oldTemp, petObjectSerial);
			}
			++iter;
		}	
		return true;
	}

	bool Player::PetUnsummon(NonPlayer* object)
	{
		std::list<Familiar>::iterator familiarIter = familiars.begin();

		for(; familiarIter != familiars.end(); ++familiarIter)
		{
			if((*familiarIter).object == object)
				break;
		}

		if(familiarIter == familiars.end())
		{
			Logger::GetInstance().Error(L"Logic::Player::PetUnsummon Failed - not exist in familiars");
			return false;
		}

		PetsIter itrPet = std::find_if(myPets.begin(), myPets.end(), PetWithSummonInfoFinder::ByPetNPCSerial(object->GetSerial()));

		if( itrPet == myPets.end() )
		{
			Logger::GetInstance().Error(L"Logic::Player::PetUnsummon Failed - not exist in appearPets");
			return false;
		}

		if( itrPet->PetNPCSerial == 0 )
		{
			Logger::GetInstance().Error(L"Logic::Player::PetUnsummon Failed - PetNPCSerial is Zero");
			return false;
		}

		(*familiarIter).object->MasterCommandMeSuicide(true);
		familiars.erase(familiarIter);

		itrPet->Pet.ExpFactor = 1.0f;
		itrPet->Pet.TiredFactor = 1.0f;
		itrPet->Pet.Appear = false;		

		const std::vector< XRated::Item > equipment( itrPet->Pet.Equipments );
		std::vector< XRated::Item >::const_iterator iter = equipment.begin();
		std::vector< XRated::Item >::const_iterator end = equipment.end();
		while( iter != end ) {
			if( iter->Id != 0 ) {
			const Database::Info::ItemInfo* itemInfo = Database::DatabaseInstance().InfoCollections.Items.Retrieve( iter->Id );			
				uint32 oldTemp;
				XRated::Serial petObjectSerial;
				PetEquip( itrPet->Pet.PetSerial, NULL, 0, XRated::Constants::GetEquipPosition(itemInfo->EquipParts), oldTemp, petObjectSerial);
			}
			++iter;
		}
		itrPet->Pet.Equipments= equipment;

		stageData->PetInfoUpdated(this, itrPet->Pet, object->GetSerial(), L"PET_UNSUMMON");

		RemovePetEffectState( itrPet->Pet );
		itrPet->PetNPCSerial = 0;

		return true;
	}

	bool Player::PetUnsummon(XRated::Pet& pet)
	{
		std::list<Familiar>::iterator familiarIter = familiars.begin();

		for(; familiarIter != familiars.end(); ++familiarIter)
		{
			if((*familiarIter).object->GetNPCInfo()->HashcodeName == pet.PetHash)
				break;
		}

		if(familiarIter == familiars.end())
		{
			Logger::GetInstance().Error(L"Logic::Player::PetUnsummon Failed - not exist in familiars");
			return false;
		}

		PetsIter itrPet = std::find_if(myPets.begin(), myPets.end(), PetWithSummonInfoFinder::ByPetNPCSerial((*familiarIter).object->GetSerial()));

		if( itrPet == myPets.end() )
		{
			Logger::GetInstance().Error(L"Logic::Player::PetUnsummon Failed - not exist in appearPets");
			return false;
		}

		if( itrPet->PetNPCSerial == 0 )
		{
			Logger::GetInstance().Error(L"Logic::Player::PetUnsummon Failed - PetNPCSerial is Zero");
			return false;
		}

		XRated::Serial petObjectSerial = (*familiarIter).object->GetSerial();
		(*familiarIter).object->MasterCommandMeSuicide(true);
		familiars.erase(familiarIter);		

		itrPet->Pet.ExpFactor = 1.0f;
		itrPet->Pet.TiredFactor = 1.0f;
		itrPet->Pet.Appear = false;		

		const std::vector< XRated::Item > equipment( itrPet->Pet.Equipments );
		std::vector< XRated::Item >::const_iterator iter = equipment.begin();
		std::vector< XRated::Item >::const_iterator end = equipment.end();
		while( iter != end ) {
			if( iter->Id != 0 ) {
			const Database::Info::ItemInfo* itemInfo = Database::DatabaseInstance().InfoCollections.Items.Retrieve( iter->Id );			
				uint32 oldTemp;
				XRated::Serial petObjectSerial;
				PetEquip( itrPet->Pet.PetSerial, NULL, 0, XRated::Constants::GetEquipPosition(itemInfo->EquipParts), oldTemp, petObjectSerial);
			}
			++iter;
		}
		itrPet->Pet.Equipments = equipment;

		stageData->PetInfoUpdated(this, itrPet->Pet, petObjectSerial, L"PET_UNSUMMON");

		RemovePetEffectState(itrPet->Pet);
		itrPet->PetNPCSerial = 0;

		return true;
	}

	void Player::UnsummonPet()
	{
		NonPlayer* npc = NULL;

		for(std::list<Familiar>::iterator itr = familiars.begin(); itr != familiars.end(); ++itr)
		{
			if(itr->type == XRated::Constants::Familiar::Type::Pet)
			{
				npc = itr->object;
				break;
			}
		}

		if(npc != NULL)
			PetUnsummon(npc);
	}

	bool Player::PetUpdate(float dt)
	{
		const PetsIter end = myPets.end();

		for(PetsIter itr = myPets.begin(); itr != end; ++itr)
		{		
			if(itr->Pet.Appear == true) {
				if(itr->Pet.Full <= 0.0f) {
					// ��ȯ���� �ϴ� ���� �ƴ� ������ ������� �Ѵ�
					//PetUnsummon(itr->Pet);
					//RemovePetEffectState( itr->Pet );
				}
				else {
					if(stageData->IsSquare() == false)
					{
						//TODO : find�� �ʹ� ����. ����ȭ�� �ؾ� ���� ������ ���� �ȴ�.
						{

							if( itr->Pet.EffectStateDelayTime > XRated::Pet::AddEffectStateTime ) {
								const Database::Info::Pet::Species* species = Database::DatabaseInstance().InfoCollections.Pets.GetSpecies( itr->Pet.PetHash );
								const Database::Info::Pet::EffectStates* effectStates = NULL;
								if( species != NULL )
								{
									const wchar_t* buff = NULL;

									// ������ Pet ��θ��� 0�� �ƴ� ��쿡 �ɾ��ش�.
									if( itr->Pet.IsRarePet == true ) 
									{
										effectStates = Database::DatabaseInstance().InfoCollections.Pets.GetRarePetEffectBuff( itr->Pet.PetHash, playerData.Job );
										buff = effectStates->GetStateBundle( 1, itr->Pet.GetBuffIndex(), species->LevelInterval, species->ArrayInterval );
									}
									else
									{
										effectStates = Database::DatabaseInstance().InfoCollections.Pets.GetNormalPetEffectBuff( itr->Pet.PetHash, playerData.Job );
										buff = effectStates->GetStateBundle( itr->Pet.Level, itr->Pet.GetBuffIndex(), species->LevelInterval, species->ArrayInterval );
									}

									if( effectStates == NULL ) 
									{
										Logger::GetInstance().Exception(L"Player::PetUpdate() - can not found effect State");
									}

									if( effectStates != NULL )
									{
										if( buff != NULL ) {
											Database::Info::StateBundleInfo* bundle = Database::DatabaseInstance().InfoCollections.StateBundles.Retrieve( buff );
											if( bundle == NULL ) {
												Logger::GetInstance().Exception(L"can not found stateBundle : [{0}]",effectStates->GetStateBundle( itr->Pet.Level, itr->Pet.GetBuffIndex(), species->LevelInterval, species->ArrayInterval ));
											}

											if( itr->PetNPCSerial != 0 )
											{
												AddStateBundle( bundle, GetSerial(), itr->PetNPCSerial );
												stageData->PetInfoUpdated(this, itr->Pet, itr->PetNPCSerial, L"PET_BUFFTOPLAYER");
											}
										}	
									}
								}
								itr->Pet.EffectStateDelayTime = 0.0f;
							}
						}
					}
				}

				if( itr->PetNPCSerial != 0 )
				{					
					if(itr->Pet.Level < Pet::MaxLevel)
					{
						bool petLevelUp = false;
						while(itr->Pet.Exp >= static_cast<float>(itr->Pet.NextLevelExp) )
						{
							itr->Pet.Exp -= static_cast<float>(itr->Pet.NextLevelExp);

							petLevelUp = true;

							PetLevelUp( itr->Pet );
							if( itr->Pet.Level >= Pet::MaxLevel ) {
								itr->Pet.Exp = 0;
								break;
							}
						}
						if( petLevelUp )
						{
							// Client�� Broadcasting
							// EffectState Delay Time ������ ������ ��Ŷ�� ������ ������ ������.. ��.. ���� ����� ������?						
							NonPlayer* npcPlayer = stageData->RetrieveNPC(itr->PetNPCSerial);
							if( npcPlayer != NULL ) {
								RemovePetEffectState( itr->Pet );
								AddPetEffectState( itr->Pet, npcPlayer );
								stageData->PetInfoUpdated(this, itr->Pet, itr->PetNPCSerial, L"PET_PETLEVELUP");
							}else {
								Logger::GetInstance().Warn(L"PetLevelUp : UpdateEffectState Failed, can not found npc : {0}", itr->PetNPCSerial );
							}
						}
					}					
				}

				if( itr->Pet.EmotionDelay <= 0.0f ) {
					if( itr->Pet.GetFeeling() == XRated::Pet::Feeling::State::Starving )
					{
						itr->Pet.EmotionDelay = static_cast<float>(Lunia::Math::Random(30,60));
					}
					else
					{
						itr->Pet.EmotionDelay = static_cast<float>(Lunia::Math::Random(30,180));
					}
					RunPetEmotion(itr->Pet.PetSerial, false);
				}else {

				}
			}
			itr->Pet.Update(dt);
		}

		return true;
	}

	void Player::PetLevelUp(XRated::Pet& pet)
	{
		if(pet.Level >= Pet::MaxLevel)
		{
			pet.Exp = 0;
			return;
		}

		const Info::Pet::Genus* genus = XRated::Database::DatabaseInstance().InfoCollections.Pets.GetGenus( pet.PetHash );
		if( genus == NULL )	return;


		if( pet.LevelUpPeriod > 0.0 )
		{
			pet.RareProbability += pet.GetAddedRareProbability( pet.FullSum / pet.LevelUpPeriod, genus->MaxRarePetProbability, genus->RareProbabilityAddPerLevel, pet.Level );
		}
		else 
		{
			pet.RareProbability += pet.GetAddedRareProbability( pet.GetFullRatio(), genus->MaxRarePetProbability, genus->RareProbabilityAddPerLevel, pet.Level );
		}

		if( pet.RareProbability > 1.0f )	pet.RareProbability = 1.0f;
		if( pet.RareProbability < 0.0f )	pet.RareProbability = 0.0f;

		pet.FullSum = 0.0f;
		pet.LevelUpPeriod = 0.0f;

		++pet.Level;
		if(pet.Level < Pet::MaxLevel) {
			pet.NextLevelExp = Database::DatabaseInstance().InfoCollections.Statuses.GetExp( Database::Info::StatusInfoManager::ExpType::Pet, pet.Level - 1 );
		}else {
			pet.NextLevelExp =0;
		}

		if(pet.Level == Pet::MaxLevel)
		{
			if(stageData->GetRandomClass()->Next() <= pet.RareProbability)
			{			
				pet.IsRarePet = true;
			}
		}
	}

	bool Player::AddPetEffectState(const XRated::Pet& pet, NonPlayer* object)
	{	
		const Info::Pet::Species*      species      = XRated::Database::DatabaseInstance().InfoCollections.Pets.GetSpecies( pet.PetHash );
		const Info::Pet::EffectStates* effectStates = NULL;

		if( pet.IsRarePet == true  )
			effectStates = Database::DatabaseInstance().InfoCollections.Pets.GetRarePetEffectState( pet.PetHash, playerData.Job );
		else
			effectStates = Database::DatabaseInstance().InfoCollections.Pets.GetNormalPetEffectState( pet.PetHash, playerData.Job );

		if( species == NULL || effectStates == NULL )
			return false;
		
		Database::Info::ActionInfo::State tempState( effectStates->GetState( pet.Level, species->LevelInterval ) );
		Stat::IState* state = Stat::CreateState<Player>(tempState.type, this, &tempState.params, &tempState.optionalStates, object->GetSerial());

		if(state != NULL)
		{
			AddState(state);

			PetEffectItr itr = petEffects.find(pet.PetSerial);
			if(itr != petEffects.end())
				itr->second.states.push_back(state);
			else
			{
				PetEffect petEffect;
				petEffect.states.push_back(state);

				petEffects.insert(std::pair<XRated::GlobalSerial, PetEffect>(pet.PetSerial, petEffect));
			}
		}

		return true;
	}
	
	bool Player::RemovePetEffectState(const XRated::Pet& pet)
	{
		PetEffectItr itr = petEffects.find(pet.PetSerial);

		if(itr != petEffects.end())
		{
			for(std::vector<Stat::IState*>::iterator stateItr = itr->second.states.begin(); stateItr != itr->second.states.end(); ++stateItr)
			{
				RemoveState((*stateItr)->GetSort(), (*stateItr)->GetId());
			}

			itr->second.states.clear();
		}

		return true;
	}

	bool Player::IsMaxPetLevel(const XRated::GlobalSerial& petSerial)
	{
		PetsIter itr = std::find_if(myPets.begin(), myPets.end(), PetWithSummonInfoFinder::ByPetSerial(petSerial));

		if(itr != myPets.end())
			return itr->Pet.Level >= XRated::Pet::MaxLevel;

		return false;
	}

	bool Player::IsRarePet(const XRated::GlobalSerial& petSerial)
	{
		PetsIter itr = std::find_if(myPets.begin(), myPets.end(), PetWithSummonInfoFinder::ByPetSerial(petSerial));

		if(itr != myPets.end())
			return itr->Pet.IsRarePet;

		return false;
	}

	void Player::GiveFoodToPet( XRated::GlobalSerial petSerial,uint32 foodAmount, uint32 foodCount, bool overFeed )
	{
		// ���⼭ ����� �������� �ٽ� ä���ָ� �ȴ�.		
		size_t i = 0, size = myPets.size();

		for( ; i < size; ++i )
		{
			if( myPets[i].Pet.PetSerial == petSerial )
			{
				bool stateUpdate = false;

				if ( myPets[ i ].Pet.Appear == true && myPets[ i ].Pet.Full <= 0.0f )
				{
					stateUpdate = true;
				}

				myPets[i].Pet.Feed( foodAmount, foodCount, overFeed );

				if ( stateUpdate && myPets[ i ].Pet.Full > 0.0f && stageData != NULL && myPets[ i ].PetNPCSerial != 0 )
				{
					NonPlayer* npcPlayer = stageData->RetrieveNPC( myPets[ i ].PetNPCSerial );

					if ( npcPlayer != NULL )
					{
						AddPetEffectState( myPets[i].Pet, npcPlayer );
					}
				}

				stageData->PetInfoUpdated(this, myPets[i].Pet, myPets[i].PetNPCSerial, L"PET_EAT");

				//����� ���¿� ���� �̸�Ƽ�� ����	
				/* // ���� �� �ϳ��� �����ϵ���, �̸�Ƽ���� ���� �÷��� �ϳ��θ�
				if( myPets[i].Pet.Appear == true ) { 
					RunPetEmotion( petSerial, false );					
				}
				*/
				break;
			}
		}
	}
	void Player::GivePresentToPet(XRated::GlobalSerial petSerial, uint32 presentSellPrice, uint32 presentItemCount)
	{
		// ���⼭ ����� �������� �ٽ� ä���ָ� �ȴ�.		
		size_t i = 0, size = myPets.size();
		for( ; i < size; ++i )
		{
			if( myPets[i].Pet.PetSerial == petSerial )
			{
				if( myPets[i].Pet.GetFullRatio() >= XRated::Pet::RelatedWithPresentNFull::PresentLimitFull )
					break;

				bool stateUpdate = false;

				if ( myPets[ i ].Pet.Appear == true && myPets[ i ].Pet.Full <= 0.0f )
				{
					stateUpdate = true;
				}

				myPets[i].Pet.Give( presentSellPrice, presentItemCount );

				if ( stateUpdate && myPets[ i ].Pet.Full > 0.0f && stageData != NULL && myPets[ i ].PetNPCSerial != 0 )
				{
					NonPlayer* npcPlayer = stageData->RetrieveNPC( myPets[ i ].PetNPCSerial );

					if ( npcPlayer != NULL )
					{
						AddPetEffectState( myPets[i].Pet, npcPlayer );
					}
				}

				stageData->PetInfoUpdated(this, myPets[i].Pet, myPets[i].PetNPCSerial, L"PET_EAT");

				//����� ���¿� ���� �̸�Ƽ�� ����
				/* // ���� �� �ϳ��� �����ϵ���, �̸�Ƽ���� ���� �÷��� �ϳ��θ�
				if( myPets[i].Pet.Appear == true ) { 
					RunPetEmotion( petSerial, false );					
				}
				*/
				break;
			}
		}
	}

	bool Player::RunPetEmotion(const XRated::GlobalSerial& petSerial, bool sendOnlyChanged)
	{
		// �̸�Ƽ�� ó���� �������� ������ ��,.��
		PetsIter itrPet = std::find_if( myPets.begin(), myPets.end(), PetWithSummonInfoFinder::ByPetSerial(petSerial) );

		if( itrPet != myPets.end() )
		{
			if( itrPet->PetNPCSerial == 0 )
			{
				return false;
			}

			if( !sendOnlyChanged || itrPet->Pet.GetFeeling() != itrPet->Pet.GetPrevFeeling() )	
			{
				switch( itrPet->Pet.GetFeeling() )
				{
				case XRated::Pet::Feeling::State::Full:
					stageData->PetInfoUpdated(this, itrPet->Pet, itrPet->PetNPCSerial, L"PET_STATE_FULL");
					break;
				case XRated::Pet::Feeling::State::Good:
					stageData->PetInfoUpdated(this, itrPet->Pet, itrPet->PetNPCSerial, L"PET_STATE_GOOD");
					break;
				case XRated::Pet::Feeling::State::Soso:
					stageData->PetInfoUpdated(this, itrPet->Pet, itrPet->PetNPCSerial, L"PET_STATE_SOSO");
					break;
				case XRated::Pet::Feeling::State::NotGood:
					stageData->PetInfoUpdated(this, itrPet->Pet, itrPet->PetNPCSerial, L"PET_STATE_NOTGOOD");
					break;
				case XRated::Pet::Feeling::State::Hungry:
					stageData->PetInfoUpdated(this, itrPet->Pet, itrPet->PetNPCSerial, L"PET_STATE_HUNGRY");
					break;
				case XRated::Pet::Feeling::State::Starving:
					stageData->PetInfoUpdated(this, itrPet->Pet, itrPet->PetNPCSerial, L"PET_STATE_STARVING");
					break;
				}
			}
			itrPet->Pet.PrevFull = itrPet->Pet.Full;
			return true;				
		}

		return false;
	}
	bool Player::PetEquip(XRated::GlobalSerial petSerial, const Database::Info::ItemInfo* info, int64 fields, Constants::Equipment where, uint32& equipmentOld, Serial& petObjectSerial, bool bForce )
	{
		//pet equip�� �̹� ��ȯ�� �༮�鸸 �Ҽ� �ֱ� ������ ��ȯ�� pet table���� �˻��Ѵ�.
		PetsIter itrPet = std::find_if( myPets.begin(), myPets.end(), PetWithSummonInfoFinder::ByPetSerial(petSerial) );

		if( itrPet != myPets.end() )
		{
			if( itrPet->PetNPCSerial == 0 )
			{
				return false;
			}

			if( inventory.PetEquip( itrPet->Pet, info, fields, where, equipmentOld, this, bForce ) == true ) {
				petObjectSerial = itrPet->PetNPCSerial;

				if( equipmentOld != 0 ) {
					//����						
					itrPet->Pet.UnEquipment(where);		
				}
				
				if( info != NULL ) {
					//����.
					itrPet->Pet.Equipment(info->Hash, fields, where);		
				}
				return true;
			}
			return false;
		}

		return false;
	}
	bool Player::ChangePetEquipToEquip(XRated::GlobalSerial petSerial, Constants::Equipment from,Constants::Equipment to)
	{		
		//TODO : Pet�� ChangePetEquipToEquip �Լ��� �ʿ� �����Ƿ� ���� ���� �ؾ� �Ѵ�.
		return false;
	}
	void Player::AddPet( const XRated::Pet& pet )
	{
		PetWithSummonInfo petWithSummonInfo;
		petWithSummonInfo.Pet = pet;
		petWithSummonInfo.PetNPCSerial = 0;
		myPets.push_back( petWithSummonInfo );
	}

	void Player::TakeCarePetBySchool( const uint32& petItemHash, const XRated::GlobalSerial& petSerial
									, const Database::Enchant::EnchantBitfields& instance, const uint8& count
		                            , const uint32& period, const float& expFactor )
	{
		PetsIter itr = std::find_if(myPets.begin(), myPets.end(), PetWithSummonInfoFinder::ByPetSerial(petSerial));

		// ��ȯ�Ǿ� �ִ� ���� �缺�ҿ� �� �ñ��~~ �������� 
		if( itr->Pet.Appear == true )	return;

		PetsCaredIter iter = std::find_if( caredPets.begin(), caredPets.end(), XRated::PetCaredBySchool::Finder( petSerial ) );

		if( iter == caredPets.end() )
		{
			// �� �缺�ҿ� �ñ� �������� �� ��θ� ���̴�!!
			if ( itr->Pet.Full < XRated::Pet::MaxFull )
			{
				itr->Pet.Full = XRated::Pet::MaxFull;
			}

			XRated::PetCaredBySchool data;
			data.PetItemHash   = petItemHash;
			data.PetItemSerial = petSerial;
			data.PetItemInstanceEx = instance;
			data.PetItemCount  = count;
			data.ExpFactor     = expFactor;
			data.Start    = Lunia::DateTime::Now();
			data.End           = data.Start;
			data.End.AddEx( Lunia::DateTime::Unit::Hour, period ); // ������ �д����� �ִ´�~ �ð������� �ٲܰ�~~

			caredPets.push_back( data );

			// Send Updated Info!!!!!!!!!!!!!!!!!!
			stageData->PetCaredBySchool( this, data, itr->Pet );
		}
		else
		{
			// �ð� �ִ� ���¿��� ���� �Ұ���!!!!!!!!!!!!!! ������������...
		}
	}

	void Player::TakeOutCaredPet( const XRated::GlobalSerial& petSerial )
	{
		PetsIter itr = std::find_if(myPets.begin(), myPets.end(), PetWithSummonInfoFinder::ByPetSerial(petSerial));

		// ��ȯ�Ǿ� �ִ� ���� �缺�ҿ� �� �ñ��~~ �������� 
		if( itr->Pet.Appear == true )	return;

		PetsCaredIter iter = std::find_if( caredPets.begin(), caredPets.end(), XRated::PetCaredBySchool::Finder( petSerial ) );

		if( iter != caredPets.end() )
		{
			// ����ġ ������ �ؼ� Updated Packet�� �����ش�~~
			Lunia::DateTime now = Lunia::DateTime::Now();

			int day;
			double second;

			if( iter->End > now ) 
			{
				day = now.GetDate().GetCumulatedDay() - iter->Start.GetDate().GetCumulatedDay();
				second = now.GetTime().GetCumulatedSec() - iter->Start.GetTime().GetCumulatedSec();
			}
			else
			{
				day = iter->End.GetDate().GetCumulatedDay() - iter->Start.GetDate().GetCumulatedDay();
				second = iter->End.GetTime().GetCumulatedSec() - iter->Start.GetTime().GetCumulatedSec();
			}

			double addExp = 0.0;
			/* // �и���ũ�� ������ ����~
			if( periodTime.GetMilliSec() != 0 )
			{
				addExp += periodTime.GetMilliSec() / Lunia::DateTime::Constant::MilliSecondsInASecond;
			}
			*/

			addExp += second + day * 86400;//24 * 60 * 60;

			// ������ üũ
			bool petLevelUp = false;

			if(itr->Pet.Level < Pet::MaxLevel)
			{
				itr->Pet.UpdateByPetSchool( addExp, iter->ExpFactor );

				if( itr->Pet.NextLevelExp == 0 )
				{
					itr->Pet.NextLevelExp = Database::DatabaseInstance().InfoCollections.Statuses.GetExp( Database::Info::StatusInfoManager::ExpType::Pet, itr->Pet.Level - 1 );
				}

				while(itr->Pet.Exp >= itr->Pet.NextLevelExp)
				{
					itr->Pet.Exp -= itr->Pet.NextLevelExp;

					// Pet Level Up�� �˸��� Broadcasting Packet �ʿ�
					petLevelUp = true;

					PetLevelUp( itr->Pet );
					if( itr->Pet.Level >= Pet::MaxLevel ) {
						itr->Pet.Exp = 0;
						break;
					}
				}
			}

			stageData->PetTakenOutFromSchool( this, *iter, itr->Pet );

			caredPets.erase( iter ); // �̰Ÿ� Item�� Acquire �� ������ �ϱ⺸��~ Inventory üũ�� ������!!
		}
	}

	void Player::PetRenaming(XRated::GlobalSerial petSerial, const std::wstring& newName)
	{
		PetsIter itr = std::find_if(myPets.begin(), myPets.end(), PetWithSummonInfoFinder::ByPetSerial(petSerial));

		if(itr == myPets.end())
			return;

		/// Appear�� true ���¿��� �� �������� ��û���� �ϱ⶧���� ���� ���⼭ Appear üũ�ϸ� ������ �� �����... 2008.11.27 by Yong-Eun, Jung
        //if(itr->Pet.Appear == false)
		//	return;

		itr->Pet.PetName = newName;

		stageData->PetInfoUpdated(this, itr->Pet, itr->PetNPCSerial, L"PET_RENAMING");		
	}

	void Player::RecalculateHP(bool bFill, bool bIgnoreLimit) 
	{
		float maxOld = status.MaxHp;
		if ( stageData->GetGameType() & XRated::Constants::GameTypes::PvpGameTypeMask)
		{
			status.MaxHp = ( Logic::impl->GetMaxHpByStat( GetClassType(), status.GetVit(), stageData->GetGameType() ) + status.hpPlus ) * status.hpMul;
			if ( bFill )
			{
				status.Hp += ( status.MaxHp - maxOld );
			}
			if ( status.Hp > status.MaxHp )
			{
				status.Hp = status.MaxHp;
			}
			else if ( status.Hp < 0 )
			{
				status.Hp = 0;
			}
		}
		else
		{
			if ( status.Vit >= 500 )
			{
				status.MaxHp = (Logic::impl->GetMaxHpByStat( GetClassType(), status.GetVit(), stageData->GetGameType() ) + status.hpPlus ) * status.hpMul;
				if ( bFill )
				{
					status.Hp += ( status.MaxHp - maxOld );
				}
				if ( status.Hp > status.MaxHp )
				{
					status.Hp = status.MaxHp;
				}
				else if ( status.Hp < 0 )
				{
					status.Hp = 0;
				}
			}
			else
			{					
				status.RecalculateHP(bFill, bIgnoreLimit); 
			}
		}
		//Actor::RecalculateHP( bFill, bIgnoreLimit );
	}

	void Player::RecalculateMP(bool bFill, bool bIgnoreLimit) 
	{
		float maxOld = status.MaxMp;
		if ( stageData->GetGameType() & XRated::Constants::GameTypes::PvpGameTypeMask)
		{
			status.MaxMp = (Logic::impl->GetMaxMpByStat( GetClassType(), status.GetInt(), stageData->GetGameType() ) + status.mpPlus ) * status.mpMul;
			if ( bFill )
			{
				status.Mp += ( status.MaxMp - maxOld );
			}
			if ( status.Mp > status.MaxMp )
			{
				status.Mp = status.MaxMp;
			}
			else if ( status.Mp < 0 )
			{
				status.Mp = 0;
			}
		}
		else
		{
			if ( status.Int >= 500 )
			{
				status.MaxMp = (Logic::impl->GetMaxMpByStat( GetClassType(), status.GetInt(), stageData->GetGameType() ) + status.mpPlus ) * status.mpMul;
				if ( bFill )
				{
					status.Mp += ( status.MaxMp - maxOld );
				}
				if ( status.Mp > status.MaxMp )
				{
					status.Mp = status.MaxMp;
				}
				else if ( status.Mp < 0 )
				{
					status.Mp = 0;
				}
			}
			else
			{					
				status.RecalculateMP(bFill, bIgnoreLimit); 
			}
		}
		//Actor::RecalculateMP( bFill, bIgnoreLimit );
	}

	void Player::RecalculateSecondStat()
	{
		Logic::impl->CalculateSecondStat( GetClassType(), status.GetBasicStat(), secondStat, stageData->GetGameType() );
		//if ( status.Vit >= 500 )
		//{
		//	status.MaxHp = ( secondStat.maxHp + status.hpPlus ) * status.hpMul;
		//	status.Hp = status.Hp > status.MaxHp ? status.MaxHp : status.Hp;			
		//}

		//if ( status.Int >= 500 )
		//{
		//	status.MaxMp = ( secondStat.maxMp + status.mpPlus ) * status.mpMul;
		//	status.Mp = status.Mp > status.MaxMp ? status.MaxMp : status.Mp;			
		//}
		ResetAllFamiliarStatusByPlayerStat();
	}

	void Player::RecalculateStatus( bool bIgnoreLimit )
	{
		if( characterStateFlags.StatusLimit == 1 )
		{
			status.MaxStatus = 1000;
			status.dmg.MaxAtk = 1000.f;
		}
		else if( characterStateFlags.StatusLimit == 2 )
		{
			status.MaxStatus = 500;
			status.dmg.MaxAtk = 500.f;
		}

		RecalculateHP( false, bIgnoreLimit );
		RecalculateMP( false, bIgnoreLimit );		
		status.dmg.Recalculate( bIgnoreLimit );
		status.Ac.Recalculate( bIgnoreLimit );
		status.prob.Recalculate( bIgnoreLimit );
		status.RecalculateAtkSpeed();
		RecalculateSecondStat();	/// by kpongky( 09.09.24 ) stat revolution.
	}

	void Player::ChangePlayerStatusLimit( std::vector<uint32>* statuses, const std::vector<uint32>& limits )
	{
		if( statuses->size() != limits.size()){
			Logger::GetInstance().Exception(L"Player::ChangeStatusLimit size of statuses({0}) doesn't match with limits({1})", statuses->size(), limits.size());
		}

		for(int index = 0; index < statuses->size(); index++){
			switch(statuses->at(index)){
				case Constants::CharacterMajorStatType::Str:
					status.LimitStr = uint32((limits.at(index) <= 0) ? XRated::Constants::MaxStat : limits.at(index));
				break;
				case Constants::CharacterMajorStatType::Dex:
					status.LimitDex = uint32((limits.at(index) <= 0) ? XRated::Constants::MaxStat : limits.at(index));
				break;
				case Constants::CharacterMajorStatType::Int:
					status.LimitInt = uint32((limits.at(index) <= 0) ? XRated::Constants::MaxStat : limits.at(index));
				break;
				case Constants::CharacterMajorStatType::Vit:
					status.LimitVit = uint32((limits.at(index) <= 0) ? XRated::Constants::MaxStat : limits.at(index));
				break;
				case 4:
					status.stackedInt = uint32((limits.at(index) <= 0) ? XRated::Constants::MaxStat : limits.at(index));
				break;
				case 5:
					status.stackedVit = uint32((limits.at(index) <= 0) ? XRated::Constants::MaxStat : limits.at(index));
				break;
			}
		}
		RecalculateStatus( );
		// here is working this stats, need recalc and send to client updated stats
		wprintf(L"str[{}], dex[{}], int[{}], vit[{}] \n", status.GetStr(), status.GetDex(), status.GetInt(), status.GetVit());
	}

	void Player::ChangePlayerStatusRate( std::vector<uint32>* statusId, const std::vector<float> & rate )
	{
		if( statusId->size() != rate.size()){
			Logger::GetInstance().Exception(L"Player::ChangeStatusLimit size of statusId({0}) doesn't match with rate({1})", statusId->size(), rate.size());
		}

		for(int index = 0; index < statusId->size(); index++){
			switch(statusId->at(index)){
				case Constants::CharacterMajorStatType::Str:
					status.StrFactor = float(rate.at(index));
				break;
				case Constants::CharacterMajorStatType::Dex:
					status.DexFactor = float(rate.at(index));
				break;
				case Constants::CharacterMajorStatType::Int:
					status.VitFactor = float(rate.at(index));
				break;
				case Constants::CharacterMajorStatType::Vit:
					status.IntFactor = float(rate.at(index));
				break;
			}
		}
		RecalculateStatus( );
		wprintf(L"str[{}], dex[{}], int[{}], vit[{}] \n", status.GetStr(), status.GetDex(), status.GetInt(), status.GetVit());
	}

	float Player::GetSkillDamageIncreaseFactorByStat() const
	{
		return secondStat.skillDamageIncreaseFactor;
	}

	float Player::GetCriticalProbabilityByStat() const
	{
		return secondStat.criticalProbabilityByDexterity;
	}

	float Player::GetCoolDownTimeReduceAmount() const
	{
		return secondStat.coolDownTimeReduceByDexterity;
	}

	float Player::GetPhysicalCriticalInc() const
	{
		return secondStat.physicalCriticalFactorByVitality + increase.criAtkInc  + Database::DatabaseInstance().InfoCollections.GeneralInfos.Retrieve().PhysicalCri;
	}

	float Player::GetMagicalCriticalInc() const
	{
		return secondStat.magicalCriticalFactorByIntelligence + increase.criMagicInc + Database::DatabaseInstance().InfoCollections.GeneralInfos.Retrieve().MagicCri;
	}

	float Player::GetHitIgnoreHpRate() const
	{
		return secondStat.hitIgnoreBySpacificStat;
	}

	float Player::GetHealIncRateByStat() const
	{
		return secondStat.healIncRate;
	}

	bool Player::IsHitEffectIgnored() const
	{
		const float hpRate = status.Hp / status.MaxHp;
		
		return ( 1.0f - GetHitIgnoreHpRate() ) <= hpRate;
	}

	const float Player::GetHitIgnoreValue() const
	{
		return (status.dmg.atkMaxPlus + status.dmg.atkMaxPlus*status.dmg.atkMultiply)/2.f + Logic::impl->GetHitIgnoreValue(GetClassType(), status.Dex, status.Vit, status.Int );
	}

	void Player::SetUserPvpRank(uint32 rank)
	{
		pvpRank = rank;
		UpdatePvpRankRwward();
	}

	void Player::UpdatePvpRankRwward()
	{
		Info::PvPRankRewardInfo rewardInfo;
		bool existRewardItem = DatabaseInstance().InfoCollections.PvPRankRewardItemInfos.GetRewardInfo( pvpRank, rewardInfo );
		if (existRewardItem)
		{
			Database::Info::StateBundleInfo::ConditionList::iterator i = rewardInfo.StateBundleConditions.begin();
			for ( ; i != rewardInfo.StateBundleConditions.end() ; ++i) {
				ProcessStateBundle(*i);
			}
		}
	}

	uint32 Player::ApplyLadderResult( uint32 opponentLadderPoint, bool isWin , XRated::PvpSubInfo::Type subInfo )
	{
		if( isWin == true && subInfo == PvpSubInfo::Type::GIVEUP )
			return playerData.LadderPoint; // �������� ��ȿȭ �ϱ�� �Ͽ���.

		float factor = ( static_cast< float >( opponentLadderPoint ) - static_cast< float >( playerData.LadderPoint ) ) / 400;
		float estimation = 1 / ( 1 + pow( 10.0f, factor ) );
		float gainedPoint = 0.0f;

		switch( subInfo )
		{
		case XRated::PvpSubInfo::NORMAL :
		case XRated::PvpSubInfo::GIVEUP :
			{
				if( isWin == true )
				{
					gainedPoint = 1.0f;
				}
			}
			break;
		case XRated::PvpSubInfo::DRAW :
			{
				gainedPoint = 0.5f;
			}
			break;
		}

		// �ݿø� �Ѵ�
		float temp = 10 * ( gainedPoint - estimation );
		int diff = static_cast< int >( temp + ( (temp>=0.0f)?0.5f:-0.5f ) );

		if( isWin == false && subInfo == PvpSubInfo::Type::GIVEUP )
			diff *= 2; // �����д� 2��� ������ ���� �Ͽ���.

		playerData.LadderPoint = playerData.LadderPoint + diff;

		return playerData.LadderPoint;
	}

	uint32 Player::GetLadderPoint() const
	{
		return playerData.LadderPoint;
	}

} } }
