	/*
    Copyright (c) 2000-, Lunia Corp. All rights reserved.
        http://www.Lunia.co.kr
            tel:82-2-888-1555
                fax:82-2-888-4414
                    mailTo:webmaster@Lunia.co.kr
*/
#include "StateAddBuff.h"
#include "StateAreaHeal.h"
#include "StateAttack.h"
#include "StateBlind.h"
#include "StateChangeAction.h"
#include "StateCharge.h"
#include "StateClear.h"
#include "StateCreateNPC.h"
#include "StateCreateObject.h"
#include "StateCreateProjectile.h"
#include "StateCreateObjectProjectile.h"
#include "StateCriAllInc.h"
#include "StateCriAtk.h"
#include "StateCriAtkInc.h"
#include "StateElementalAtk.h"
#include "StateCriMagic.h"
#include "StateCriMagicInc.h"
#include "StateCriRush.h"
#include "StateDamage.h"
#include "StateDefence.h"
#include "StateDestroyEnemy.h"
#include "StateDetectInvisible.h"
#include "StateDevineShd.h"
#include "StateDot.h"
#include "StateDotMP.h"
#include "StateChoke.h"
#include "StateFreeze.h"
#include "StateHate.h"
#include "StateIgnoreDmg.h"
#include "StateIgnoreStun.h"
#include "StateImmune.h"
#include "StateIncLife.h"
#include "StateIncStat.h"
#include "StateIncSkillLevel.h"
#include "StateInvincible.h"
#include "StateKnockBack.h"
#include "StateLessMP.h"
#include "StateMagicAmr.h"
#include "StateMakeStun.h"
#include "StateMinimize.h"
#include "StateMorph.h"
#include "StateMove.h"
#include "StateReflection.h"
#include "StateRegen.h"
#include "StateResetCooldown.h"
#include "StateResist.h"
#include "StateResurrection.h"
#include "StateReverseCmd.h"
#include "StateRoot.h"
#include "StateSelfHeal.h"
#include "StateSleep.h"
#include "StateSlow.h"
#include "StateSpreadHate.h"
#include "StateStep.h"
#include "StateTaunt.h"
#include "StateWeaken.h"
#include "StateIgnoreState.h"
#include "StateIncrease.h"
#include "StateIgnoreCriticalHit.h"
#include "StateResetSkillGroupCooldown.h"
#include "StateDecreaseSkillCooldown.h"
#include "StateDmgToHpMp.h"
#include "StateAbsorbToHpMp.h"
#include "StateInventFame.h"
#include "StateGoldGain.h"
#include "States.h"

#include "../Action/ActionStun.h"
#include "../Action/ActionDown.h"
#include "../Action/ActionSleep.h"
#include "../Action/ActionPhoenixDownAir.h"
#include "../Action/ActionPhoenixDown2Air.h" //@ORACLE 01JAN2020 13:36 EST
#include "../Action/ActionCenterMove.h" //@ORACLE 01JAN2020 19:52 EST
#include "../Action/ActionStepBack.h"
#include "../GameStateDatabase.h"

#include <Core/Utils/StringUtil/GenericConversion.h>
#include "../Object/NonPlayer.h"
#include "../Object/TombStone.h"
#include "../Object/Misc/Misc.h"

#include <Info/Info/Database.h>
#include <iostream>
//#include "../Stage/Stage.h"

#include "../LogicDB/LogicDB.h"

#include <Info/Info/ETC/Balance.h>

using namespace Lunia::StringUtil;
using namespace Lunia::Math;
using namespace Lunia::XRated::Database;

namespace Lunia { namespace XRated {	namespace Logic { namespace Stat {

	void AddBuffWhenHit::Initialize(IGameStateDatabase* db)
	{
		randomFloat = db->GetRandomClass();
		bundleInfo = DatabaseInstance().InfoCollections.StateBundles.Retrieve(bundleHash);
	}

	bool AddBuffWhenHit::IsActivated()
	{
		if ( randomFloat->Next() <= probability)
			return true;
		return false;
	}

	void AddBuffWhenHit::Do(Object* target, IGameStateDatabase* db)
	{
		switch( target->GetType() ) {
		case Constants::ObjectType::NonPlayer :
		case Constants::ObjectType::Player :
			if ( IsActivated() && bundleInfo ) {
				Actor* actor = (Actor*)target;
				if ( bSelf ) {
					if ( owner->AddStateBundle(bundleInfo,owner->GetSerial()) )
						db->StateChanged(owner->GetSerial(), bundleInfo->hash, bundleInfo->level);
				} else {
					if ( actor->AddStateBundle(bundleInfo,owner->GetSerial()) )
						db->StateChanged(actor->GetSerial(), bundleInfo->hash, bundleInfo->level);
				}
			}
			break;
		}
	}

	void AddBuffWhenHitted::Initialize(IGameStateDatabase* db)
	{
		randomFloat = db->GetRandomClass();
		bundleInfo = DatabaseInstance().InfoCollections.StateBundles.Retrieve(bundleHash);
	}

	bool AddBuffWhenHitted::IsActivated()
	{
		if ( randomFloat->Next() <= probability)
			return true;
		return false;
	}

	void AddBuffWhenHitted::Do(Object* target, IGameStateDatabase* db)
	{
		if ( bSelf ) {
			switch(owner->GetType()) {
			case Constants::ObjectType::NonPlayer :
			case Constants::ObjectType::Player :
				if ( owner->AddStateBundle(bundleInfo,owner->GetSerial()) )
					db->StateChanged(owner->GetSerial(), bundleInfo->hash, bundleInfo->level);
				break;
			}
		} else {
			if ( target ) {
				Actor* actor = (Actor*)target;
				if ( actor->AddStateBundle(bundleInfo,owner->GetSerial()) )
					db->StateChanged(actor->GetSerial(), bundleInfo->hash, bundleInfo->level);
			}
		}
	}

	AreaHeal::AreaHeal(Object* owner, float r, float rate, FillType ft, Constants::AllianceType type)
		: State<Object>(owner, Database::Info::StateInfo::Type::AREAHEAL, Database::Info::StateInfo::Sort::ORDINARYSTATE)
		, range(r), regenRate(rate), fillType(ft), allianceType(type)
	{
	}

	bool AreaHeal::Do(float, IGameStateDatabase* db)
	{
		Object* realOwner = NULL;		
		std::vector<Actor*> l;

		// range, regen rate
		if(owner->GetType() == Constants::ObjectType::Projectile)
		{
			realOwner = db->RetrievePlayer(static_cast<Projectile*>(owner)->GetCreatorSerial());
			

			//Player�� ���ٸ� NonPlayer���� üũ.
			if(realOwner == NULL) realOwner = db->RetrieveNPC(static_cast<Projectile*>(owner)->GetCreatorSerial());
		}

		if(realOwner == NULL)
			realOwner = owner;

		db->GetAreaObject(owner->GetPosition(), range, allianceType, realOwner->GetTeam(), owner, l);
		
		std::vector<Actor*>::iterator i;

		for (i = l.begin() ; i != l.end() ; ++i) {
			if ( !(*i)->CheckState(Database::Info::StateInfo::Type::GHOST) && !(*i)->CheckState(Database::Info::StateInfo::Type::ZOMBIE) ) {
				CharacterData& objData = (*i)->GetCharacterData();
				
				float max;
				bool isHpEmpty = (*i)->GetHP(max) <= 0.0f ? true : false;

				float realRegenRate = regenRate;

				if(realOwner->GetType() == XRated::Constants::ObjectType::Player)
				{
					//int intelligence, temp;

					//static_cast<Player*>(realOwner)->GetBaseStat(temp, temp, intelligence, temp);
					//realRegenRate *= XRated::StatusFunctions::GetIncreasedHealRate(intelligence);

					if(db->GetStageGroupInfo()->GameType & XRated::Constants::GameTypes::PvpGameTypeMask &&
						static_cast<Player*>(realOwner)->GetCharacterStateFlags().IsBalanced == true)
						realRegenRate = regenRate * Database::BalancerInstance().GetValue(XRated::Constants::ClassType::Healer, Database::IBalancer::HealingFactor, static_cast<Player*>(realOwner)->GetLevel());

					//õĪ�� ������ �����ٰ� �÷��̾��� ���ɿ� ���� �������� ��������ֱ� ���� ���⼭ �����Ѵ�.
					realRegenRate *= static_cast<Player*>(realOwner)->GetHealIncRateByStat();
				}
				
				switch ( fillType ) {
				case FillType::Hp :
					if(realOwner->IsCharacter() == true && (*i)->GetHP() < max)
					{
						realRegenRate += owner->CalculrateIncHealAmount(realRegenRate);

						static_cast<Actor*>(realOwner)->ProcessCondition(Database::Info::StateInfo::IFHEAL, (*i));
					}

					(*i)->SetHP((*i)->GetHP() + realRegenRate);
					break;
				case FillType::HpRate :
					if(realOwner->IsCharacter() == true && (*i)->GetHP() < max)
					{
						realRegenRate += owner->CalculrateIncHealAmount(realRegenRate);

						static_cast<Actor*>(realOwner)->ProcessCondition(Database::Info::StateInfo::IFHEAL, (*i));
					}

					(*i)->SetHP((*i)->GetHP() + ( realRegenRate * max) );
					break;
				case FillType::Mp :
					(*i)->SetMP((*i)->GetMP() + realRegenRate);
					break;
				case FillType::MpRate :
					{
						float maxMp;
						(*i)->GetMP( maxMp );
						(*i)->SetMP((*i)->GetMP() + ( realRegenRate * maxMp ) );
					}
					break;
				}

				

				switch( realOwner->GetType() ) {
				case Constants::ObjectType::NonPlayer :
				case Constants::ObjectType::Player :
				case Constants::ObjectType::Vehicle :
				case Constants::ObjectType::Structure :
					db->StatusChanged(objData.BaseObject.GameObjectSerial, objData.BaseObject.Position, objData.BaseObject.Direction, objData.Hp,
						objData.Mp, static_cast<Actor*>(realOwner)->GetSerial(), static_cast<Actor*>(realOwner)->GetActionNameHash(), 0);
					break;
				case Constants::ObjectType::Projectile :
				default :
					;
				}

				if(isHpEmpty == true && (*i)->GetHP() > 0.0f)
				{
					if(db->GetStageGroupInfo()->GameType & XRated::Constants::GameTypes::CooperativeGameTypeMask && (*i)->GetActionNameHash() != 40872058)// Database::Info::HashTable::Actions::DownEnd)
						continue;

					if((*i)->GetSerial() != realOwner->GetSerial()) {
						db->PlayerSave((*i)->GetSerial(), realOwner->GetSerial());

						Player* p = db->RetrievePlayer(realOwner->GetSerial());
						if (p) {
							p->AddFantasicSaveStylePoint();
						}
					}
				}
			}
		}
		return true;
	}


	void Blind::Initialize(IGameStateDatabase* /*db*/)
	{
		if ( owner->GetType() == Constants::ObjectType::NonPlayer )
            ((NonPlayer*)owner)->ClearHateList();
	}

	void ChangeAction::Initialize(IGameStateDatabase* db)
	{
		std::vector<Actor*> l;
		switch(type) {
		case Database::Info::StateInfo::RangeType::Self :
			owner->InitHeightInfo();
			owner->ReserveAction(actionHash, NULL, bForce);
			return;
		case Database::Info::StateInfo::RangeType::Circle :
			db->GetAreaObject(owner->GetPosition(), range, applyTo, owner->GetTeam(), owner, l);
			for ( std::vector<Actor*>::iterator i = l.begin() ; i != l.end() ; ++i ) {
				Actor* actor = (*i);				
				if( (actor->CheckState(Info::StateInfo::Type::FLIED) == false) && (actor->CheckState(Info::StateInfo::Type::GHOST) == false)&& (actor->CheckState(Info::StateInfo::Type::DOWN) == false) ) {
					switch(actor->GetType()) {
					case Constants::ObjectType::NonPlayer :
						if ( targetHash == 0 ) {
							actor->InitHeightInfo();
							if ( actor == owner )
								actor->ReserveAction(actionHash, NULL, bForce);
							else
								actor->SetAction(actionHash, NULL, bForce);
						} else if ( targetHash == actor->GetNameHash() ) {
							actor->InitHeightInfo();
							if ( actor == owner )
								actor->ReserveAction(actionHash, NULL, bForce);
							else
								actor->SetAction(actionHash, NULL, bForce);
						}
						break;
					case Constants::ObjectType::Player :
						if ( targetHash == 0 ) {
							actor->InitHeightInfo();
							if ( actor == owner )
								actor->ReserveAction(actionHash, NULL, bForce);
							else
								actor->SetAction(actionHash, NULL, bForce);
						}
						break;
					}
				}
			}
			break;
		default : //�ٸ� Ÿ���� �������� �ʴ´�.
			return;
		}
		//owner->ReserveAction(actionName, NULL, true);
	}

	void ChangeAction::Destroy()
	{
		//if ( bBundle && owner->GetActionNameHash() == actionName ) {
		//	owner->ReserveAction(Database::Info::HashTable::Actions::Stand, NULL, true);
		//}
	}

	void Charge::Initialize(IGameStateDatabase* db)
	{
		float max;
		float current;
		Lunia::XRated::CharacterData& data = owner->GetCharacterData();
		switch(category) {
		case Category::HP :
			if(owner->CheckState(Database::Info::StateInfo::Type::CAMPFIRE) == true && amount < 0)
				break;

			current = owner->GetHP(max);
			if (chargeType == ChargeType::AMOUNT)
				owner->SetHP(current+amount, max);
			else
				owner->SetHP(current+max*amount, max);
			db->StatusChanged(data.BaseObject.GameObjectSerial, data.BaseObject.Position, data.BaseObject.Direction,
				data.Hp, data.Mp, data.BaseObject.GameObjectSerial, Info::HashTable::Object::itemHpPotion);
			break;
		case Category::MP :
			current = owner->GetMP(max);
			if (chargeType == ChargeType::AMOUNT)
				owner->SetMP(current+amount, max);
			else
				owner->SetMP(current+max*amount, max);
			db->StatusChanged(data.BaseObject.GameObjectSerial, data.BaseObject.Position, data.BaseObject.Direction,
								data.Hp, data.Mp, data.BaseObject.GameObjectSerial, Info::HashTable::Object::itemMpPotion);
			break;
		case Category::DASH :
			// TODO : �̰� player���� �ۿ� ����.

			//current = owner->GetDashGauge(max);
			//if (chargeType == ChargeType::AMOUNT)
			//	owner->SetDashGauge(current+amount, max);
			//else
			//	owner->SetDashGauge(current+max*amount/100, max);
			break;
		}
	}

	bool Charge::Do(float /*dt*/, IGameStateDatabase* /*db*/)
	{
		return true;
	}


	bool Clear::Do(float, IGameStateDatabase* db)
	{
		std::vector<Actor*> l;
		db->GetAreaObject(owner->GetPosition(), range, Constants::AllianceType::Team, owner->GetTeam(), owner, l);
		std::vector<Actor*>::iterator i;
		for (i = l.begin() ; i != l.end() ; ++i)
			(*i)->Debuff(buffRate, sort, subCategoryHash);

		return true;
	}

	bool CreateNPC::Do(float, IGameStateDatabase* db)
	{
		Info::NonPlayerInfo* npcInfo = DatabaseInstance().InfoCollections.Npcs.Retrieve(name);
		if ( !npcInfo ) {
			//Lunia_INFO(( L"[CreateNPC::Do] npc [{}] info not founded in database.", name ));
			return true;
		}

		if (db->GetNPCCnt() >= DatabaseInstance().InfoCollections.GeneralInfos.Retrieve().MaxNPCCnt) {
			Logger::GetInstance().Info( L"Exception [CreateNPCLLDo] Too many NPC [{0}]. Create NPC Aborted.[{1}] ", db->GetNPCCnt(), npcInfo->DisplayName.c_str() );
			return true;
		}

		float3 pos = owner->GetPosition();
		pos.y = 0;
		ObjectData& data = owner->GetObjectData();
		float3 spinnedPos = PhysicalFunctions::RotatePositionByDirection(float3(startPos.x, 0, startPos.y), 0, data.Direction);
		pos += spinnedPos;

		if ( team != 9999 )
		{
			LogicDBInstance().CreateNPC(db, name, pos, owner->GetDirection(), team, (Info::StageInfo::Npc::IdleStateType)idleType, 0, 0, owner->GetSerial(), (Info::StageInfo::Npc::TransferType)transType, db->GetMaxTeamPlayerCnt());
		}
		else
		{
			LogicDBInstance().CreateNPC(db, name, pos, owner->GetDirection(), owner->GetTeam(), (Info::StageInfo::Npc::IdleStateType)idleType, 0, 0, owner->GetSerial(), (Info::StageInfo::Npc::TransferType)transType, db->GetMaxTeamPlayerCnt());
		}
		return true;
	}

	bool CreateObject::Do(float, IGameStateDatabase* db)
	{
		float3 pos = owner->GetPosition();
		pos.y = 0;
		float3 spinnedPos = PhysicalFunctions::RotatePositionByDirection(float3(startPos.x, 0, startPos.y), 0, owner->GetDirection());
		pos += spinnedPos;

		LogicDBInstance().CreateMisc(db, name, pos, owner->GetDirection(), owner->GetTeam(), duration, (float)hp);
		return true;
	}

	void CreateProjectile::Initialize(IGameStateDatabase*)
	{
		if( minDmg == 0 || maxDmg == 0 )
			owner->GetDamage(minDmg, maxDmg);
	}

	bool CreateProjectile::Do(float, IGameStateDatabase* db)
	{
		db->CreateProjectile(name, position, fireAngle, type, owner, minDmg, maxDmg, toMyTarget, createTime, owner->GetCreatorSerial() );
		return true;
	}

	void CreateObjectProjectile::Initialize(IGameStateDatabase*)
	{
		if( minDmg == 0 || maxDmg == 0 )
			owner->GetDamage(minDmg, maxDmg);
	}

	bool CreateObjectProjectile::Do(float, IGameStateDatabase* db)
	{
		db->CreateProjectile(name, position, fireAngle, type, owner, minDmg, maxDmg, toMyTarget, createTime, owner->GetSerial() );
		return true;
	}

	void CriAllInc::Initialize( IGameStateDatabase* )
	{
		owner->AddCriAtkInc( amount );
		owner->AddCriMagicInc( amount );
	}

	void CriAllInc::Destroy()
	{
		owner->AddCriAtkInc( -amount );
		owner->AddCriMagicInc( -amount );
	}

	void CriAtk::Initialize(IGameStateDatabase*)
	{
		owner->AddCriAtk(probability);
	}

	void CriAtk::Destroy()
	{
		owner->AddCriAtk(-probability);
	}

	void CriAtkInc::Initialize( IGameStateDatabase* )
	{
		owner->AddCriAtkInc( amount );
	}

	void CriAtkInc::Destroy()
	{
		owner->AddCriAtkInc( -amount );
	}

	void ElementalAtk::Initialize(IGameStateDatabase*)
	{
		owner->AddStateFlag( Database::Info::StateInfo::Type::ELEMENTALATK );
	}
	void ElementalAtk::Destroy()
	{
		owner->RemoveStateFlag( Database::Info::StateInfo::Type::ELEMENTALATK );
	}

	//bool CriAtk::IsActivated()
	//{
	//	if (bCriRushActivated) {
	//		return true;
	//	}
	//	if ( randomFloat->Next() <= probability)
	//		return true;
	//	return false;
	//}

	void CriMagic::Initialize(IGameStateDatabase*)
	{
		owner->AddCriMagic(probability);
	}

	void CriMagic::Destroy()
	{
		owner->AddCriMagic(-probability);
	}

	void CriMagicInc::Initialize( IGameStateDatabase* )
	{
		owner->AddCriMagicInc( amount );
	}

	void CriMagicInc::Destroy()
	{
		owner->AddCriMagicInc( -amount );
	}

	//bool CriMagic::IsActivated()
	//{
	//	if ( randomFloat->Next() <= probability)
	//		return true;
	//	return false;
	//}

	void CriRush::Initialize(IGameStateDatabase*)
	{
		owner->CriticalRushActivated(true);
	}

	void CriRush::Destroy()
	{
		owner->CriticalRushActivated(false);
	}

	void Damage::Initialize(Projectile* projectile, IGameStateDatabase* db, uint32 damageStateTypeUniqueSerial )
	{
		bool bAbsoluteDmg = true;

		if ( !targetType )
			targetType = Constants::AttackTargetType::Stand;
		if ( minDmg == 0 || maxDmg == 0 )
		{
			projectile->GetDamage(minDmg, maxDmg);
			bAbsoluteDmg = false;
		}

		creatorSerial = projectile->GetCreatorSerial();
		creatorActionHash = projectile->GetActionHash();
		direction = projectile->GetDirection();

		//For increasing magic damage
		switch(dmgType) {
		case Constants::DamageType::PHYSICAL :
			break;
		default :
			{ //���� �������� �ƴ϶��.. ���ݿ� ���ؼ� ��Ƣ�� -0-
				float incFactor = 1.0f;
				incFactor = ((Projectile*)projectile)->GetIncFactor();

				Actor* creator = db->RetrieveActor( creatorSerial );

				//���ݷ¿� ���� ��ų���ݷ� ��º��� �����ش�.
				if (bAbsoluteDmg)
				{
					if(creator != NULL && creator->GetType()==Constants::ObjectType::Player)
					{
						float min=0, max=0;
						creator->GetPlusDamage(min, max);
						if (db->GetGameType() & XRated::Constants::GameTypes::PvpGameTypeMask)
							incFactor += StatusFunctions::GetIncreasedDamageRateByAtkInPvp(min + max);
						else
							incFactor += StatusFunctions::GetIncreasedDamageRateByAtk(min + max);
					}
				
					minDmg*= incFactor;
					maxDmg*= incFactor;
				}
				else
				{
					if(creator != NULL && creator->GetType()==Constants::ObjectType::Player)
					{
						//��Ÿ ���ݷ��� ��ų�������� �⺻�������϶�
						if (maxDmg > 9000) 
						{
							float maxDmgGap = maxDmg - 9000;
							maxDmg = 9000;
							minDmg -= maxDmgGap;
						}
					}

					minDmg *= incFactor;
					maxDmg *= incFactor;
				}
			}
		}

		minDmg += projectile->CalculrateIncDamage(dmgType, minDmg);
		maxDmg += projectile->CalculrateIncDamage(dmgType, maxDmg);

		/* Air Attack Combo ����.. */
		this->damageStateTypeUniqueSerial = damageStateTypeUniqueSerial;
	}

	bool Damage::Do(float dt, IGameStateDatabase* db)
	{
		currentTime += dt;
		if (currentTime >= delay) {
			if( owner != NULL )
			{
				if ( owner->CheckState(Info::StateInfo::Type::GHOST) || owner->CheckState(Info::StateInfo::Type::INVINCIBLE) || owner->GetHeightInfo().height > 15.0f )
					return true;
			}	

			//����� ���¸� ���� �Ǵ�����.
			int type = Constants::AttackTargetType::Stand;
			if ( owner != NULL && owner->CheckState(Database::Info::StateInfo::Type::FLIED) )
				type = Constants::AttackTargetType::Air;
			else if ( owner != NULL && owner->CheckState(Database::Info::StateInfo::Type::DOWN) )
				type = Constants::AttackTargetType::Ground;

			if ( !(targetType & type) ) return true; //������ target type �߿��� ������ ���°� ���ٸ� �н�.

			switch( type ) { 
			case Constants::AttackTargetType::Ground :
				if ( owner != NULL && owner->GetHP() <= 0 ) //hp�� 0 �����γ��� hp�� ���� �Ѿ��� �÷��̾�. �������̴� ������ ����.
					return true;
			}

			//��밡 �� �ϴ°�
			if ( owner != NULL && owner->IsGuarding( StatConstants::AttackEffectType::Type::PROJECTILE ) ) {
				if ( owner->IsGuardActivated( StatConstants::AttackEffectType::Type::PROJECTILE ) ) {
					owner->SetAction( Database::Info::HashTable::Actions::GuardProjectile, NULL, true);
					return true;
				}
			} else if ( owner != NULL && owner->IsGuarding( (int)atkEffectType ) ) {
				if ( owner->IsGuardActivated( (int)atkEffectType ) ) {
					switch (atkEffectType) {
					case StatConstants::AttackEffectType::Type::AIRSTUN :
						owner->SetAction( Database::Info::HashTable::Actions::GuardAirStun, NULL, true);
						break;
					case StatConstants::AttackEffectType::Type::DOWN :
						owner->SetAction( Database::Info::HashTable::Actions::GuardDown, NULL, true);
						break;
					case StatConstants::AttackEffectType::Type::FAINT :
						owner->SetAction( Database::Info::HashTable::Actions::GuardFaint, NULL, true);
						break;
					case StatConstants::AttackEffectType::Type::NOSTUN :
						owner->SetAction( Database::Info::HashTable::Actions::GuardNoStun, NULL, true);
						break;
					case StatConstants::AttackEffectType::Type::PHOENIXDOWN :
						owner->SetAction( Database::Info::HashTable::Actions::GuardPhoenixDown, NULL, true);
						break;
					case StatConstants::AttackEffectType::Type::STANDFAINT :
						owner->SetAction( Database::Info::HashTable::Actions::GuardStandFaint, NULL, true);
						break;
					case StatConstants::AttackEffectType::Type::STEPBACK :
						owner->SetAction( Database::Info::HashTable::Actions::GuardStepBack, NULL, true);
						break;
					case StatConstants::AttackEffectType::Type::STUN :
						owner->SetAction( Database::Info::HashTable::Actions::GuardStun, NULL, true);
						break;
					}
					return true;
				}
			}

			float dmg = minDmg + (db->GetRandomFloat())*(maxDmg - minDmg);
			currentTime -= delay;

			unsigned int sFlag = 0;

			////////////////////////////////////////////////////////////////////////////////////////////////////////
			//ũ��Ƽ�� ����.
			Actor* actor = NULL;
			if ( creatorSerial < GameStateDatabase::NPCSerialBase ) { //player
				actor = db->RetrievePlayer(creatorSerial);
			} else if ( creatorSerial < GameStateDatabase::ProjectileSerialBase ) { //nonplayer
				actor = db->RetrieveNPC(creatorSerial);
			}

			if ( actor ) {
				if( owner != NULL && owner->IsIgnoreCriHit(db) == false)
				{
					switch(dmgType)
					{
					case Constants::DamageType::PHYSICAL : //����
					case Constants::DamageType::INDEPENDENCE : //���Ӽ�
						if(actor->IsCriAtk(db))
						{
							dmg *= Database::DatabaseInstance().InfoCollections.GeneralInfos.Retrieve().PhysicalCri;
							sFlag |= Constants::StatusFlag::Critical;
						}
						break;
					default ://����
						if(actor->IsCriMagic(db))
						{
							dmg *= actor->GetMagicalCriticalInc();
							sFlag |= Constants::StatusFlag::CriticalMagic;
						}
						break;
					}
				}

				///	õĪ ��... �����ؾ��� �Ф�
				if(db->GetStageGroupInfo()->GameType & XRated::Constants::GameTypes::PvpGameTypeMask)
				{
					if(actor->GetType() == XRated::Constants::ObjectType::Player && static_cast<Player*>(actor)->GetCharacterStateFlags().IsBalanced)
					{
						minDmg *= Database::BalancerInstance().GetValue(static_cast<XRated::Constants::ClassType>(actor->GetClass()), Database::IBalancer::DamageFactor, actor->GetLevel());
						maxDmg *= Database::BalancerInstance().GetValue(static_cast<XRated::Constants::ClassType>(actor->GetClass()), Database::IBalancer::DamageFactor, actor->GetLevel());
						dmg *= Database::BalancerInstance().GetValue(static_cast<XRated::Constants::ClassType>(actor->GetClass()), Database::IBalancer::DamageFactor, actor->GetLevel());
					}
					else	if(actor->GetType() == XRated::Constants::ObjectType::NonPlayer)
					{
						Player* creator = db->RetrievePlayer(static_cast<XRated::Logic::NonPlayer*>(actor)->GetOwnerSerial());

						if(creator != NULL && creator->GetCharacterStateFlags().IsBalanced)
						{
							minDmg *= Database::BalancerInstance().GetValue(static_cast<XRated::Constants::ClassType>(creator->GetClass()), Database::BalancerInstance().DamageFactor, creator->GetLevel());
							maxDmg *= Database::BalancerInstance().GetValue(static_cast<XRated::Constants::ClassType>(creator->GetClass()), Database::BalancerInstance().DamageFactor, creator->GetLevel());
							dmg *= Database::BalancerInstance().GetValue(static_cast<XRated::Constants::ClassType>(creator->GetClass()), Database::BalancerInstance().DamageFactor, creator->GetLevel());
						}
					}
				}
			}
			
			bool isHpleft = false;
			
			if( owner != NULL && owner->GetCharacterData().Hp > 0.0f)
				isHpleft = true;

			if ( owner != NULL && !owner->DoDamage(db->Retrieve(creatorSerial), creatorActionHash, minDmg, maxDmg, dmg, dmgType, damageStateTypeUniqueSerial, Constants::AttackStateType::DAMAGE, sFlag) )
				return true;

			if( owner != NULL )
			{
                owner->ProcessStateWhenHitted(NULL, dmg, sFlag);
			}

			if ( false == ( sFlag & Constants::StatusFlag::HitIgnore ) )	/// ���������� ó��. Ŭ�� ó���� sFlag�� �̿��ؼ� Ŭ�󿡼� �Ѵ�.
			{
				switch (atkEffectType) {
					case StatConstants::AttackEffectType::CENTERMOVE: //@oracle 01JAN2020 19:53 EST. needs implementation
					case StatConstants::AttackEffectType::PHOENIXDOWN2: //@oracle 01JAN2020 13:37 EST. needs implementation
					case StatConstants::AttackEffectType::PHOENIXDOWN :
					//Action::ActionPhoenixDownAir::Param *param = new Action::ActionPhoenixDownAir::Param(knockBack, Constants::DirectionF::GetDirection(direction ) * -1);
					//Action::ActionParam p(param, sizeof(Action::ActionPhoenixDownAir::Param));
					break;
				case StatConstants::AttackEffectType::STUN :
					if ( owner != NULL && owner->IsIgnoreStun(StatConstants::AttackEffectType::StunFlag) )
						break;
					switch ( type ) {
					case Constants::AttackTargetType::Air :
						HitAir();
						break;
					case Constants::AttackTargetType::Stand :
						Stun();
						break;
					case Constants::AttackTargetType::Ground :
						if( owner != NULL )
						{
							owner->SetAction( Database::Info::HashTable::Actions::HitDown, NULL, true);
						}
						break;
					}
					break;
				case StatConstants::AttackEffectType::AIRSTUN :
					if ( owner != NULL && owner->IsIgnoreStun(StatConstants::AttackEffectType::AirStunFlag) )
						break;
					switch ( type ) {
					case Constants::AttackTargetType::Air :
						HitAir();
						break;
					case Constants::AttackTargetType::Stand :
					case Constants::AttackTargetType::Ground :
						AirStun();
						break;
					}
					break;
				case StatConstants::AttackEffectType::DOWN :
					if ( owner != NULL && owner->IsIgnoreStun(StatConstants::AttackEffectType::DownFlag) )
						break;
					switch ( type ) {
					case Constants::AttackTargetType::Air :
						HitAir();
						break;
					case Constants::AttackTargetType::Stand :
					case Constants::AttackTargetType::Ground :
						if( owner != NULL )
						{
							Action::ActionDownStart::Param param(20, owner->GetDirection(), float3(0, 0, 0));
							owner->SetAction( Database::Info::HashTable::Actions::DownStart, &param, true);
						}
						break;
					}
					break;
				case StatConstants::AttackEffectType::STEPBACK :
					if ( owner != NULL && owner->IsIgnoreStun(StatConstants::AttackEffectType::StepBackFlag) )
						break;
					switch ( type ) {
					case Constants::AttackTargetType::Air :
						HitAir();
						break;
					case Constants::AttackTargetType::Stand :
					case Constants::AttackTargetType::Ground :
						StepBack();
						break;
					}
				}
			}

			if(db->GetStageGroupInfo()->GameType & XRated::Constants::GameTypes::PvpGameTypeMask &&
				owner != NULL && owner->GetCharacterData().Hp <= 0.0f)
			{
				if(actor)
				{
					if(actor->GetType() == XRated::Constants::ObjectType::Player)
					{
						if(((owner->GetActionNameHash() == Database::Info::HashTable::Actions::HitAir ||
							// DownAir
							owner->GetActionNameHash() == 40868073)						&& isHpleft == true)	||
							owner->GetActionNameHash() == Database::Info::HashTable::Actions::PhoenixDownAir	||
							owner->GetActionNameHash() == Database::Info::HashTable::Actions::PhoenixDown2Air	|| //@oracle 01JAN2020 13:37 EST
							owner->GetActionNameHash() == Database::Info::HashTable::Actions::DownStart			||
							owner->GetActionNameHash() == Database::Info::HashTable::Actions::FaintDownStart)
						{
							Logger::GetInstance().Warn(L"[Damage::Do] objectSerial {0} fromSerial {1}", owner->GetSerial(), actor->GetSerial());
							db->PlayerDie(owner->GetSerial(), actor->GetSerial());
						}
					}
					else	if(actor->GetType() == XRated::Constants::ObjectType::NonPlayer)
					{
						if(((owner->GetActionNameHash() == Database::Info::HashTable::Actions::HitAir ||
							// DownAir
							owner->GetActionNameHash() == 40868073)						&& isHpleft == true)	||
							owner->GetActionNameHash() == Database::Info::HashTable::Actions::PhoenixDownAir	||
							owner->GetActionNameHash() == Database::Info::HashTable::Actions::PhoenixDown2Air	|| //@ORACLE 01JAN2020 13:37 EST
							owner->GetActionNameHash() == Database::Info::HashTable::Actions::DownStart			||
							owner->GetActionNameHash() == Database::Info::HashTable::Actions::FaintDownStart)
						{
							Logger::GetInstance().Warn(L"[Attack::Process] objectSerial {0} fromSerial(nonplayer) {1}", owner->GetSerial(), static_cast<XRated::Logic::NonPlayer*>(actor)->GetOwnerSerial());
							db->PlayerDie(owner->GetSerial(), static_cast<XRated::Logic::NonPlayer*>(actor)->GetOwnerSerial());
						}
					}
				}

				//Air Combo Clear
				//((Actor*)owner)->ClearAirAttackData();
			}
		}
		return true;
	}

	void Damage::HitAir()
	{
		float3 targetDirection = owner->GetDirection() * -1;
		Action::ActionHitAir::Param param(airMakeFly, owner->GetDirection(), direction*airKnockBack);
		owner->SetAction( Database::Info::HashTable::Actions::HitAir, &param, true);
	}

	void Damage::Stun()
	{
		Action::ActionStun::Param param(stunLength, direction*knockBack);
		owner->SetAction( Database::Info::HashTable::Actions::HitBack, &param, true);
	}

	void Damage::AirStun()
	{
		Action::ActionDownStart::Param param(makeFly, owner->GetDirection(), direction*knockBack);
		owner->SetAction( Database::Info::HashTable::Actions::DownStart, &param, true);
	}

	void Damage::StepBack()
	{
		Action::ActionStepBack::Param param(knockBack, direction * -1);
		owner->SetAction(Database::Info::HashTable::Actions::MovingKnockBack, &param, true);
	}

	void Defence::Initialize(IGameStateDatabase*)
	{
		owner->AddGuardProbability(type, probability);
	}

	void Defence::Destroy()
	{
		owner->RemoveGuardProbability(type, probability);
	}

	void DestroyEnemy::DestroyNPC(IGameStateDatabase* db)
	{
		std::vector<NonPlayer*> l;
		switch(areaType) {
		case AreaType::ONE :
			break;
		case AreaType::RANGE :
			db->GetAreaObject(owner->GetPosition(), radius, Constants::AllianceType::ExTeam, owner->GetTeam(), owner, l);
			break;
		case AreaType::SCREEN :
			{
				float3 pos = owner->GetPosition();
				db->GetAreaObjectRect(float4(pos.x-80, pos.z+60, pos.x+80, pos.z-60), l);
			}
			break;
		case AreaType::ALLINGAME :
			break;
		}

		std::vector<NonPlayer*>::iterator i;
		for (i = l.begin() ; i != l.end() ; ++i) {
			if ( (int)(*i)->GetNPCType() > 0 ) //�븻 ���Ͱ� �ƴ϶�� ������ ���� �ʴ´�.
				continue;
			(*i)->SetHP(0);
		}
	}

	void DestroyEnemy::DestroyProjectile(IGameStateDatabase* db)
	{
		std::vector<Projectile*> l;
		switch(areaType) {
		case AreaType::ONE :
		case AreaType::SCREEN :
		case AreaType::ALLINGAME :
			break;
		case AreaType::RANGE :
			db->GetAreaObject(owner->GetPosition(), radius, Constants::AllianceType::ExTeam, owner->GetTeam(), owner, l);
			break;
		}

		std::vector<Projectile*>::iterator i;
		for (i = l.begin() ; i != l.end() ; ++i) {
			(*i)->Destroy();
		}
	}

	bool DestroyEnemy::Do(float /*dt*/, IGameStateDatabase* db)
	{
		switch ( targetType ) {
		case Constants::ObjectType::Projectile :
			DestroyProjectile(db);
			break;
		case Constants::ObjectType::NonPlayer :
			DestroyNPC(db);
			break;
		}

		return true;
	}


	void DevineShd::Initialize(IGameStateDatabase*)
	{
		//stateInvincible = new Invincible(owner);
		//owner->AddState(stateInvincible);
		owner->AddStateFlag( Database::Info::StateInfo::Type::INVINCIBLE );
	}

	bool DevineShd::Do(float dt, IGameStateDatabase* db)
	{
		State<Actor>::Do(dt, db);
		if (duration > 0 && passedTime > duration)
			return true;
		return false;
	}

	void DevineShd::Destroy()
	{
		//stateInvincible->Remove();
        owner->RemoveStateFlag( Database::Info::StateInfo::Type::INVINCIBLE );
	}

	void DevineShd::Remove()
	{
		if ( owner->RemoveStateBundle(this) )
			return;
		duration = passedTime;
	}

	void Dot::Initialize(IGameStateDatabase*)
	{
		//if (dmgType == Constants::DamageType::PHYSICAL)
		//	dps = owner->CalculatedDmg(dps);
		//else {
		//	float reducedDmg =  dps - owner->GetResist(dmgType);

		//	dps = (reducedDmg < dps/2 ? dps/2 : reducedDmg);
		//}
	}

	bool Dot::Do(float dt, IGameStateDatabase* db)
	{
		State<Actor>::Do(dt, db);

		tick += dt;
		if ( tick > interval ) {
			tick -= interval;
			float max;
			float hp;
			hp = owner->GetHP(max);

			//ObjectData& data = owner->GetObjectData();
			if (owner->CheckState(Database::Info::StateInfo::Type::INVINCIBLE) ||
				owner->CheckState(Database::Info::StateInfo::Type::CAMPFIRE) ||
				owner->IsImmune( dmgType ) ) {
				// TODO : immune�� ���� ������ client���� ó�� �Ҽ� ���°�.
				//float mp, mpMax;
				//mp = owner->GetMP(mpMax);
				//db->StatusChanged(data.GameObjectSerial, data.Position, data.Direction, hp/max, mp/mpMax, 0, L"", Constants::StatusFlag::Immune);
			} else {
				hp = owner->GetHP(max);
				hp -= dps;
				hp = hp > max ? max : hp;
				hp = hp < 0 ? 0 : hp;
				owner->ChangeHpFromState(hp,0,GetCreator());
			}
		}

		if (duration > 0 && duration < passedTime)
			return true;
		return false;
	}

	void DotMP::Initialize(IGameStateDatabase*)
	{
		//if (dmgType == Constants::DamageType::PHYSICAL)
		//	dps = owner->CalculatedDmg(dps);
		//else {
		//	float reducedDmg =  dps - owner->GetResist(dmgType);

		//	dps = (reducedDmg < dps/2 ? dps/2 : reducedDmg);
		//}
	}

	bool DotMP::Do(float dt, IGameStateDatabase* db)
	{
		State<Actor>::Do(dt, db);

		tick += dt;
		if ( tick > interval ) 
		{
			tick -= interval;
			float max;
			float mp;
			mp = owner->GetMP(max);

			if (owner->CheckState(Database::Info::StateInfo::Type::INVINCIBLE) ||
				owner->CheckState(Database::Info::StateInfo::Type::CAMPFIRE) )
			{
			} 
			else 
			{
				mp = owner->GetMP(max);
				mp -= dps;
				mp = mp > max ? max : mp;
				mp = mp < 0 ? 0 : mp;
				owner->SetMP(mp, 0);
			}
		}

		if (duration > 0 && duration < passedTime)
			return true;
		return false;
	}

	void Choke::Initialize(IGameStateDatabase*)
	{
	}

	bool Choke::Do(float dt, IGameStateDatabase* db)
	{
		State<Actor>::Do(dt, db);

		tick += dt;
		if ( tick > interval ) {
			tick -= interval;
			float max;
			float hp;
			hp = owner->GetHP(max);

			//ObjectData& data = owner->GetObjectData();
			if (owner->CheckState(Database::Info::StateInfo::Type::INVINCIBLE) ||
				owner->CheckState(Database::Info::StateInfo::Type::CAMPFIRE) ||
				owner->CheckState(Database::Info::StateInfo::Type::DOWN) ||
				owner->IsImmune( dmgType ) ) {
				// TODO : immune�� ���� ������ client���� ó�� �Ҽ� ���°�.
				//float mp, mpMax;
				//mp = owner->GetMP(mpMax);
				//db->StatusChanged(data.GameObjectSerial, data.Position, data.Direction, hp/max, mp/mpMax, 0, L"", Constants::StatusFlag::Immune);
			} else {
				hp = owner->GetHP(max);
				// choke�� dps�� �޸� max�� ���� %�� �Ǹ� ��´�
				hp -= (max * dps);
				hp = hp > max ? max : hp;
				hp = hp < 0 ? 0 : hp;
				// hp�� 0�� �Ǹ� �׾�� �Ѵ�.
				owner->SetHP( hp );

				if ( hp <= 0 && owner->GetType() == Constants::ObjectType::Player ) 
				{
					Action::ActionDownStart::Param param(20, owner->GetDirection(), float3(0, 0, 0));
					owner->SetAction( Database::Info::HashTable::Actions::DownStart, &param, true);
				}
				//owner->ChangeHpFromState(hp,0,GetCreator());
			}
		}

		if (duration > 0 && duration < passedTime)
			return true;
		return false;
	}

	void Freeze::Initialize(IGameStateDatabase*)
	{
		//if ( owner->
		owner->PauseAction(-1);
	}
	void Freeze::Destroy()
	{
		owner->RemoveReservedAction();
		owner->PauseAction(0);
	}

	void Hate::Initialize(IGameStateDatabase*)
	{
		if ( owner->GetType() == Constants::ObjectType::NonPlayer )
            ((NonPlayer*)owner)->ClearHateList();
	}

	void IgnoreDmg::Initialize(IGameStateDatabase* db)
	{
		randomFloat = db->GetRandomClass();
	}

	bool IgnoreDmg::IsActivated()
	{
		if ( randomFloat->Next() <= probability)
			return true;
		return false;
	}

	void IgnoreCriticalHit::Initialize(IGameStateDatabase*)
	{
		owner->AddIgnoreCriHit(probability);
	}

	void IgnoreCriticalHit::Destroy()
	{
		owner->AddIgnoreCriHit(-probability);
	}

	void IgnoreStun::Initialize(IGameStateDatabase* db)
	{
		switch ( useState )
		{
		case 0:
			if (db->GetGameType() & XRated::Constants::GameTypes::PvpGameTypeMask )
			{
				bUse = false;
				return;
			}
			break;
		default:
			bUse = true;
			break;
		}

		if ( bIgnore )
			owner->SetIgnoreStun(flag);
		else
			owner->RemoveIgnoreStun(flag);
	}

	void IgnoreStun::Destroy()
	{
		if (bUse)
			owner->RemoveIgnoreStun(flag);
	}

	void Immune::Initialize(IGameStateDatabase* /*db*/)
	{
		owner->SetImmune(immune);
	}

	void Immune::Destroy()
	{
		owner->RemoveImmune(immune);
	}

	bool IncLife::Do(float , IGameStateDatabase* db)
	{
		if ( owner->GetType() == Constants::ObjectType::Player ) {
			Player* player = (Player*)owner;
			player->IncLife(life);
			db->Revived(player);
		}
		return true;
	}

	void IncStat::Initialize(IGameStateDatabase*)
	{
		Enable(true);

		//if (owner->GetType() == Constants::ObjectType::Player)
		//{
		//	switch(category)
		//	{
		//	case Category::STR:
		//	case Category::DEX:
		//	case Category::INT:
		//	case Category::VIT:
		//		break;
		//	default:
		//		break;
		//	}
		//}
	}

	void IncStat::Destroy()
	{
        Enable(false);
		
		//if (owner->GetType() == Constants::ObjectType::Player)
		//{
		//	switch(category)
		//	{
		//	case Category::STR:
		//	case Category::DEX:
		//	case Category::INT:
		//	case Category::VIT:
		//		break;
		//	default:
		//		break;
		//	}
		//}
	}

	void IncStat::Enable(bool enable)
	{
		// Do not invoke Player::VerifyEquippedItems() in here.
        if(enable)
		{
			if(stateEnable == ENABLE)
			{
				return;
			}

			stateEnable = ENABLE;

			switch(category) 
			{
			case Category::MAXHP :
				if (chargeType == ChargeType::AMOUNT)
					owner->AddHP(amount);
				else
					owner->AddHPMul(amount);
				owner->RecalculateHP(bFill, owner->GetType()==Constants::ObjectType::NonPlayer ? true : false);
				break;
			case Category::MAXMP :
				if (chargeType == ChargeType::AMOUNT)
					owner->AddMP(amount);
				else
					owner->AddMPMul(amount);
				owner->RecalculateMP(bFill, owner->GetType()==Constants::ObjectType::NonPlayer ? true : false);
				break;
			case Category::ATKMAX :
				if (chargeType == ChargeType::AMOUNT)
					owner->AddDamage(0, amount);
				else
					owner->AddDamage(amount);
				owner->RecalculateDmg(owner->GetType()==Constants::ObjectType::NonPlayer ? true : false);
				break;
			case Category::ATKMIN :
				if (chargeType == ChargeType::AMOUNT)
					owner->AddDamage(amount, 0);
				else
					owner->AddDamage(amount);
				owner->RecalculateDmg(owner->GetType()==Constants::ObjectType::NonPlayer ? true : false);
				break;
			case Category::DEF :
				if (chargeType == ChargeType::AMOUNT)
					owner->AddAc(amount);
				else
					owner->AddAcMul(amount);
				owner->RecalculateAc(owner->GetType()==Constants::ObjectType::NonPlayer ? true : false);
				break;
			case Category::STR :
				if( chargeType == ChargeType::AMOUNT )
					owner->AddStr((int)amount);
				else
					owner->AddStrFactor( amount );
                owner->RecalculateSecondStat();
				owner->RecalculateDmg(owner->GetType()==Constants::ObjectType::NonPlayer ? true : false);				
				break;
			case Category::INT :
				if( chargeType == ChargeType::AMOUNT )
                    owner->AddInt((int)amount);
				else
					owner->AddIntFactor( amount );
				owner->RecalculateSecondStat();
				owner->RecalculateMP(bFill, owner->GetType()==Constants::ObjectType::NonPlayer ? true : false);				
				break;
			case Category::DEX :
				if( chargeType == ChargeType::AMOUNT )
                    owner->AddDex((int)amount);
				else
					owner->AddDexFactor( amount );
				owner->RecalculateSecondStat();
				owner->RecalculateDmg(owner->GetType()==Constants::ObjectType::NonPlayer ? true : false);
				owner->RecalculateAc(owner->GetType()==Constants::ObjectType::NonPlayer ? true : false);				
				break;
			case Category::VIT :
				if( chargeType == ChargeType::AMOUNT )
                    owner->AddVit((int)amount);				
				else
					owner->AddVitFactor( amount );
				owner->RecalculateSecondStat();
				owner->RecalculateHP(bFill, owner->GetType()==Constants::ObjectType::NonPlayer ? true : false);
				break;
			case Category::ATKSPD :
				if ( chargeType == ChargeType::AMOUNT )
				{
					owner->AddAttackSpeed( amount );
				}
				else
				{
					owner->AddAttackSpeedRate( amount );
				}
				owner->RecalculateAtkSpeed();
				break;
			}

			//Lunia_IMPORTANT(("[State Enable] Category : {}, Amount : {}, ", category, (int)amount));
		}
		else
		{
			if(stateEnable == DISABLE || stateEnable == NEEDINIT)
			{
				return;
			}

			stateEnable = DISABLE;

			switch(category) 
			{
			case Category::MAXHP :
				if (chargeType == ChargeType::AMOUNT)
					owner->AddHP(-amount);
				else
					owner->AddHPMul(-amount);
				break;
			case Category::MAXMP :
				if (chargeType == ChargeType::AMOUNT)
					owner->AddMP(-amount);
				else
					owner->AddMPMul(-amount);
				break;
			case Category::ATKMAX :
				if (chargeType == ChargeType::AMOUNT)
					owner->RemoveDamage(0, amount);
				else
					owner->RemoveDamage(amount);
				break;
			case Category::ATKMIN :
				if (chargeType == ChargeType::AMOUNT)
					owner->RemoveDamage(amount, 0);
				else
					owner->RemoveDamage(amount);
				break;
			case Category::DEF :
				if (chargeType == ChargeType::AMOUNT)
					owner->RemoveAc(amount);
				else
					owner->RemoveAcMul(amount);
				break;
			case Category::STR :
				if( chargeType == ChargeType::AMOUNT )
                    owner->AddStr((int)-amount);
				else
					owner->AddStrFactor( -amount );
				break;
			case Category::INT :
				if( chargeType == ChargeType::AMOUNT )
                    owner->AddInt((int)-amount);
				else
					owner->AddIntFactor( -amount );
				break;
			case Category::DEX :
				if( chargeType == ChargeType::AMOUNT )
					owner->AddDex((int)-amount);
				else
					owner->AddDexFactor( -amount );
				break;
			case Category::VIT :
				if( chargeType == ChargeType::AMOUNT )
					owner->AddVit((int)-amount);
				else
					owner->AddVitFactor( -amount );
				break;
			case Category::ATKSPD :
				if ( chargeType == ChargeType::AMOUNT )
				{
					owner->AddAttackSpeed( -amount );
				}
				else
				{
					owner->AddAttackSpeedRate( -amount );
				}
				break;
			}
			owner->RecalculateStatus(owner->GetType()==Constants::ObjectType::NonPlayer ? true : false);
			//Lunia_IMPORTANT(("[State Enable] Category : {}, Amount : {}, ", category, (int)-amount));
		}
	}

	void IncSkillLevel::Initialize(IGameStateDatabase* /*db*/)
	{
		owner->IncSkillLevel(skillHash, level, false);
	}

	void IncSkillLevel::Destroy()
	{
		owner->DecSkillLevel(skillHash, (uint8)level, false);
	}

	bool Invincible::Do(float dt, IGameStateDatabase* db)
	{
		State<Object>::Do(dt, db);
		if (duration > 0 && passedTime >= duration)
			bDestroy = true;
		return bDestroy;
	}

	void KnockBack::Initialize(IGameStateDatabase* db)
	{
		float3 pos = owner->GetPosition();
		pos += direction * distance;
		bool bAbletoLocate = db->IsAbletoLocate(owner->GetSerial(), pos, owner->GetRadius(), owner);
		if (bAbletoLocate)
			owner->SetPosition(pos);

		if ( (!owner->IsCollide() && !bAbletoLocate) || (owner->IsCollide() && bAbletoLocate) ) {
			owner->ChangeCollisionState();
			db->CollisionStateChanged(owner->GetSerial(), owner->IsCollide(), owner->GetPosition());
		}
	}

	bool KnockBack::Do(float /*dt*/, IGameStateDatabase* /*db*/)
	{
		return true;
	}

	void LessMP::Initialize(IGameStateDatabase*)
	{
		owner->AddLessMp(probability);
	}

	void LessMP::Destroy()
	{
		owner->AddLessMp(-probability);
	}

	void MagicAmr::Initialize(IGameStateDatabase*)
	{
		owner->AddAc(defence);
	}

	bool MagicAmr::Do(float dt, IGameStateDatabase* db)
	{
		State<Actor>::Do(dt, db);
		currentTime += dt;
		if (currentTime > Lunia::XRated::Constants::Tick*2) {
			currentTime -= Lunia::XRated::Constants::Tick*2;
			float max, hp = owner->GetHP(max);
			if ( hp <= 0 )
				return false;
			owner->SetHP(regenRate+hp);
		}

		return false;
	}

	void MagicAmr::Destroy()
	{
		owner->RemoveAc(defence);
	}


	bool MakeStun::Do(float /*dt*/, IGameStateDatabase* db)
	{
		Actor* actor;
		std::vector<Actor*> l;
		if (bArea) {//���� �������ΰ�. �ܹ߼��ΰ�.
			db->GetAreaObject(owner->GetPosition(), range, Constants::AllianceType::ExTeam, owner->GetTeam(), owner, l);
			for (std::vector<Actor*>::iterator i = l.begin(), end = l.end(); i != end ; ++i ) {
				actor = (*i);
				if ( actor->GetHeightInfo().height > 0 || actor->CheckState(Info::StateInfo::Type::DOWN) ||
					actor->CheckState(Info::StateInfo::Type::INVINCIBLE) )
					continue;

				Action::ActionStun::Param param(stunLength, float3(0,0,0));
				actor->SetAction( Database::Info::HashTable::Actions::HitFront, &param, true);
			}
		} else {
			db->GetAreaObject(owner->GetPosition(), range, Constants::AllianceType::ExTeam, owner->GetTeam(), owner, l);
			for (std::vector<Actor*>::iterator i = l.begin(), end = l.end(); i != end ; ++i ) {
				actor = (*i);
				if ( actor->GetHeightInfo().height > 0 || actor->CheckState(Info::StateInfo::Type::DOWN) ||
					actor->CheckState(Info::StateInfo::Type::INVINCIBLE) )
					continue;

				Action::ActionStun::Param param(stunLength, float3(0,0,0));
				actor->SetAction( Database::Info::HashTable::Actions::HitFront, &param, true);
				break;
			}
		}
		return true;
	}

	void Minimize::Initialize(IGameStateDatabase*)
	{
		owner->AddMinimize(probability);
	}

	void Minimize::Destroy()
	{
		owner->AddMinimize(-probability);
	}

	void Morph::Initialize(IGameStateDatabase* /*db*/)
	{
		Database::Info::NonPlayerInfo* info = DatabaseInstance().InfoCollections.Npcs.Retrieve(objectHash);
		if ( !info ) {
#ifdef _SINGLE
			Lunia_IMPORTANT((L"[Morph::Initialize] Unable to find npc info. [{}]", objectHash));
#endif
			return;
		}
		Action::Action::ActionPtrList* actionList = LogicDBInstance().GetActionDB().GetActionList(info->HashcodeTemplateName, info->HashcodeActionListName);
		if (!actionList) {
#ifdef _SINGLE
			Lunia_IMPORTANT((L"[Morph::Initialize] Unable to find action list. [{}/{}]", info->HashcodeTemplateName, info->HashcodeActionListName));
#endif
			return;
		}
		switch ( owner->GetType() ) {
		case Constants::ObjectType::Player :
		case Constants::ObjectType::NonPlayer :
			oldActionList = owner->SetActionList(actionList, info->StartActionHash);
			break;
		}
	}

	void Morph::Destroy()
	{
		if ( !oldActionList )  //���̿� ���� �ߴ�.
			return;

		switch ( owner->GetType() ) {
		case Constants::ObjectType::Player :
			owner->SetActionList(oldActionList, Database::Info::HashTable::Actions::Stand);
			owner->ClearMorphStateBundleInfo(objectHash);
			break;
		case Constants::ObjectType::NonPlayer :
			{
				Database::Info::NonPlayerInfo* info = DatabaseInstance().InfoCollections.Npcs.Retrieve(owner->GetNameHash());
				if ( info )
					owner->SetActionList(oldActionList, info->StartActionHash);
				else //��� �̷� ���� �����ϸ� �ȵǱ�����������.. �ϴ� ���� ���ƾ� �ϴ�..
					owner->SetActionList(oldActionList, Database::Info::HashTable::Actions::Stand);
			}
			break;
		}
	}

	void Move::Initialize(IGameStateDatabase*)
	{
		float3 objDir = owner->GetDirection();
		float3 directionF;

		Constants::Direction dir = Constants::DirectionF::GetDirectionEnum(objDir);

		switch ( dir ) {
		case Constants::Direction::Up : break;
		case Constants::Direction::RightUp : direction += 1; break;
		case Constants::Direction::Right : direction += 2; break;
		case Constants::Direction::RightDown : direction += 3; break;
		case Constants::Direction::Down : direction += 4; break;
		case Constants::Direction::LeftDown : direction += 5; break;
		case Constants::Direction::Left : direction += 6; break;
		default : direction += 7;
		}
		if (direction > 7) direction -= 8;

		switch (direction) {
		case 0 :
			directionF = Lunia::XRated::Constants::DirectionF::Up;
			break;
		case 1 :
			directionF = Lunia::XRated::Constants::DirectionF::RightUp;
			break;
		case 2 :
			directionF = Lunia::XRated::Constants::DirectionF::Right;
			break;
		case 3 :
			directionF = Lunia::XRated::Constants::DirectionF::RightDown;
			break;
		case 4 :
			directionF = Lunia::XRated::Constants::DirectionF::Down;
			break;
		case 5 :
			directionF = Lunia::XRated::Constants::DirectionF::LeftDown;
			break;
		case 6 :
			directionF = Lunia::XRated::Constants::DirectionF::Left;
			break;
		default :
			directionF = Lunia::XRated::Constants::DirectionF::LeftUp;
		}

		directionVector = directionF * speed;
	}

	bool Move::Do(float dt, IGameStateDatabase* db)
	{
		if (pauseTime > 0) {
			//������ ���¶�� ������ �ð��� ������ ���� �ƹ��͵� ���� �ʴ´�.
			if (pauseTime - dt <= 0) {//���� ���´� Ǯ����.
				dt -= pauseTime;
				pauseTime = 0;
			} else {//������ ���� ����
				pauseTime -= dt;
				return false;
			}
		}

		if (owner->CheckState(Info::StateInfo::Type::ROOT) || owner->CheckState(Info::StateInfo::Type::FREEZE))	//��Ʈ�� �ɷ����� �������� ���Ѵ�.
			return false;

		float3 pos = owner->GetPosition();
		pos += directionVector * dt * owner->GetSpeedFactor();

		bool bAbletoLocate;
		if (owner->CheckState(Info::StateInfo::Type::GHOST))//���ɻ��¸� �ɸ��ʹ� ���.
			bAbletoLocate = db->IsAbletoLocateByGhost(pos, owner->GetRadius());
		else
			bAbletoLocate = db->IsAbletoLocate(owner->GetSerial(), pos, owner->GetRadius(), owner);

		if (bAbletoLocate)
			owner->SetPosition(pos);

		if ( (!owner->IsCollide() && !bAbletoLocate) || (owner->IsCollide() && bAbletoLocate) ) {
			owner->ChangeCollisionState();
			db->CollisionStateChanged(owner->GetSerial(), owner->IsCollide(), owner->GetPosition());
		}
		return false;
	}


	void RegenHp::Initialize(IGameStateDatabase* /*db*/)
	{
		if (chargeType == ChargeType::RATE) {
			float max;
			owner->GetHP(max);
			speed = max*speed;
		}
	}

	bool RegenHp::Do(float dt, IGameStateDatabase* db)
	{
		State<Actor>::Do(dt, db);
		elapsed += dt;
		if (elapsed >= tick) {
			elapsed -= tick;
			float maxHp, maxMp;
			float hp = owner->GetHP(maxHp);
			if ( (isRegenWhenZero == false) && (hp <= 0) ) // �������̿� �������� ����
				return false;
			if( hp <= 0 ) {
				hp += speed;
				db->StatusChanged(owner->GetSerial(), owner->GetPosition(), owner->GetDirection(), (hp > maxHp ? maxHp : hp), owner->GetMP(maxMp), owner->GetSerial(), owner->GetActionNameHash());
			}else {
				hp += speed;
			}			
			//Lunia_IMPORTANT((L"____________HP : %f, MaxHP : %f____________", hp, max));
			owner->SetHP(hp, maxHp);
			//db->StatusChanged(owner->GetSerial(), Lunia::XRated::Constants::StatusElement::CurrentHp, (int)hp, (int)max, L"");
		}
		return false;
	}


	void RegenMp::Initialize(IGameStateDatabase* /*db*/)
	{
		if (chargeType == ChargeType::RATE) {
			float max;
			owner->GetMP(max);
			speed = max*speed;
		}
	}

	bool RegenMp::Do(float dt, IGameStateDatabase* db)
	{
		State<Actor>::Do(dt, db);
		elapsed += dt;
		//tmp += dt;
		if (elapsed >= tick) {
			elapsed -= tick;
			float max, hp = owner->GetHP(max);
			float mp = owner->GetMP(max);
			if ( (isRegenWhenZero == false) && (hp <= 0) ) // �������̿� �������� ����
				return false;
			mp += speed;
			owner->SetMP(mp, max);
			//tmp2 += speed;
			//db->StatusChanged(owner->GetSerial(), Lunia::XRated::Constants::StatusElement::CurrentHp, (int)hp, (int)max, L"");
		}
		return false;
	}

	void RegenMp::Destroy()
	{
	}

	bool ResetCooldown::Do(float, IGameStateDatabase* db)
	{
		uint32 serial = owner->GetSerial();
		switch( type ) {
		case Database::Info::StateInfo::RangeType::Self :
			owner->ResetCoolDown(serial);
			break;
		case Database::Info::StateInfo::RangeType::Circle :
			{
				std::vector<Player*> l;
				std::vector<Player*>::iterator i;
				db->GetAreaObject(owner->GetPosition(), range, Constants::AllianceType::Team, owner->GetTeam(), owner, l);
				for (i = l.begin() ; i != l.end() && targetCount ; ++i) {
					if ( (*i) != owner ) {
						(*i)->ResetCoolDown(serial);
						--targetCount;
					}
				}
			}
			break;
		default :
			Logger::GetInstance().Warn( L"[ResetCooldown::Do] ResetCooldown state cannot have type [{0}]", type );
		}
		return true;
	}

	bool ResetSkillGroupCooldown::Do(float, IGameStateDatabase*)
	{
		owner->ResetSkillGroupCoolDown(skillGroupHash);
		return true;
	}

	void DecreaseSkillCooldown::Initialize(IGameStateDatabase*)
	{
		owner->AddSkillCooldownTime(skillGroupHash, amount);
	}

	void DecreaseSkillCooldown::Destroy()
	{
		owner->AddSkillCooldownTime(skillGroupHash, -amount);
	}

	void Resist::Initialize(IGameStateDatabase* /*db*/)
	{
		owner->AddResist(rate, (int)dmgType);
	}

	void Resist::Destroy()
	{
		owner->RemoveResist(rate, (int)dmgType);
	}

	bool Resurrection::Do(float, IGameStateDatabase* db)
	{
		std::cout << "ol Resurrection::Do(float, IGameStateDatabase* db)" << std::endl;
		std::vector<Player*> l;
		db->GetAreaObject(owner->GetPosition(), owner->GetDirection(), TombStone::ReviveRange, 6, Constants::AllianceType::Team, owner->GetTeam(), owner, l);
		std::vector<Player*>::iterator i;

		for (i = l.begin() ; i != l.end() ; ++i) {
			if ((*i) == owner || !(*i)->CheckState(Info::StateInfo::Type::GHOST))
				continue;

			(*i)->Revive( Player::ReviveStatus::Skill, false, refillRate, false);

			return true;
		}

		{
			std::vector<Object*> l;
			db->GetAreaObject(owner->GetPosition(), owner->GetDirection(), 20, 6, owner, l);
			std::vector<Object*>::iterator i;
			for (i = l.begin() ; i != l.end() ; ++i) {
				if ( (*i)->GetType() == Constants::ObjectType::TombStone ) {
					Player* player = db->RetrievePlayer( ((TombStone*)(*i))->GetOwnerSerial() );
					if ( player && player->GetTeam() == owner->GetTeam() )
						player->Revive( Player::ReviveStatus::Skill, false, refillRate, true);
				}
			}
		}
		return true;
	}

	void ReverseCmd::Initialize(IGameStateDatabase*)
	{
		//���� ������Ű�� ���õȰ��� �ƴϴ�.
		//if ( owner->IsMoving() ) { //�����̰� �ִ�. ������ �ٲ�����.
		//	owner->Command(Action::Action::CommandType::MOVE, );
		//}
	}

	bool Root::Do(float dt, IGameStateDatabase* db)
	{
		State<Actor>::Do(dt, db);
		if ( owner->GetHeight() > 0 ) {//���߿� ����.
			if ( owner->RemoveStateBundle(this) )
				return false; // ������ ���� �Ѵٸ� ���鿡�� ������ ���̹Ƿ� true�� �����ؼ� �̸� ���ŵ��� �ʰ� �Ѵ�.
			return true;
		}
		return false;
	}

	bool SelfHeal::Do(float, IGameStateDatabase* db)
	{
		float max, realAmount = amount, hp = owner->GetHP(max);

		//if(db->GetStageGroupInfo()->GameType & XRated::Constants::GameTypes::PvpGameTypeMask	&&
		//	owner->GetType() == XRated::Constants::ObjectType::Player)
		if(owner->GetType() == XRated::Constants::ObjectType::Player)
		{
			if(owner->CheckState(Database::Info::StateInfo::Type::GHOST) || owner->CheckState(Database::Info::StateInfo::Type::ZOMBIE))
				return true;

			if(static_cast<Player*>(owner)->GetCharacterStateFlags().IsBalanced == true)
			{
				Logger::GetInstance().Warn(L"[SelfHeal::Do] objectSerial {0} fromSerial(projectile) {1}", owner->GetSerial(), owner->GetSerial());
				realAmount *= Database::BalancerInstance().GetValue(static_cast<XRated::Constants::ClassType>(static_cast<Player*>(owner)->GetClass()), Database::IBalancer::HealingFactor, static_cast<Player*>(owner)->GetLevel());
			}
			///	�ڱ��ڽ��� �� �Ҷ��� ȯŸ��ƽ���̺갡 �ȳ������� �Ѵ�.
			//////	�Ʒ� �ּ� Ǯ���ָ� ���������� ȯŸ��ƽ ���̺갡 ����
			///if(hp <= 0.0f)	/// && owner->GetActionNameHash() == Database::Info::HashTable::Actions::Down)
			///	db->PlayerSave(owner->GetSerial(), owner->GetSerial());

			//int intelligence, temp;

			//static_cast<Player*>(owner)->GetBaseStat(temp, temp, intelligence, temp);

			//realAmount *= XRated::StatusFunctions::GetIncreasedHealRate(intelligence);

			realAmount *= owner->GetHealIncRateByStat();
		}

		realAmount += owner->CalculrateIncHealAmount(realAmount);
		
		if(hp < max)
			owner->ProcessCondition(Database::Info::StateInfo::IFHEAL, owner);

		hp += realAmount;
		owner->SetHP(hp, max);
		db->StatusChanged(owner->GetSerial(), owner->GetPosition(), owner->GetDirection(), (hp > max ? max : hp), owner->GetMP(max), owner->GetSerial(), owner->GetActionNameHash());

		return true;
	}

	bool Sleep::Do(float, IGameStateDatabase* db)
	{
		CharacterData& data = owner->GetCharacterData();
		std::vector<Actor*> l;
		db->GetAreaObject(data.BaseObject.Position, range, Constants::AllianceType::ExTeam, data.Team, owner, l);
		std::vector<Actor*>::iterator i;

		Actor* actor;
		for (i = l.begin() ; i != l.end() ; ++i) {
			actor = (*i);
			//�������̳� �Ѿ����ų�, ���߿� ���ִ³��� sleep �� ������ ���� �ʴ´�.
			if (actor->CheckState(Info::StateInfo::Type::FLIED) || actor->CheckState(Info::StateInfo::Type::DOWN) ||
				actor->CheckState(Info::StateInfo::Type::GHOST) || actor->CheckState(Info::StateInfo::Type::INVINCIBLE) )
						continue;

			//sleep�� ��� �ɸ��� Ȯ���� curse resist�� ������ ���� �ʵ��� ���� �Ѵ�. 2007.12.10 by kwind.
			float probability = curse;//(curse - actor->GetResist( (int)Constants::DamageType::CURSE ));

			if ( db->GetRandomFloat() > probability ) // �����ߴ�.
				continue;

			Action::ActionSleep::Param param(duration, buffRate, Action::ActionSleep::Direction::LEFT);
			actor->SetAction( Database::Info::HashTable::Actions::Sleep, &param, true);
		}

		return true;
	}

	void Slow::Initialize(IGameStateDatabase* /*db*/)
	{
		rate = owner->AddSpeedFactor(rate);
	}

	void Slow::Destroy()
	{
		owner->AddSpeedFactor(-rate);
	}

	const float SpreadHate::Tick = 1.0f;

	bool SpreadHate::Do(float dt, IGameStateDatabase* db)
	{
		// TODO : �ӽ÷� ���⼭ ����Ʈ�� �Ѹ��� ��.
		time += dt;
		if (time >= Tick) {
			time -= Tick;
			std::vector<Actor*> l;
			db->GetAreaObject(owner->GetPosition(), range, Constants::AllianceType::ExTeam, owner->GetTeam(), owner, l);
			if ( !l.empty() ) {
				std::vector<Actor*>::iterator i = l.begin();
				std::vector<Actor*>::iterator end = l.end();
				for(; i != end ; ++ i) {
					if ( (*i)->GetType() == Constants::ObjectType::NonPlayer) {
						((NonPlayer*)(*i))->ChangeHate(owner->GetSerial(), hateRate);
					}
				}
			}
		}
		return false;
	}

	void Step::Initialize(IGameStateDatabase* /*db*/)
	{
		float3 objDir = owner->GetDirection();
		float3 directionF;

		Constants::Direction dirEnum = Constants::DirectionF::GetDirectionEnum(objDir);

		switch ( dirEnum ) {
		case Constants::Direction::Up : break;
		case Constants::Direction::RightUp : direction += 1; break;
		case Constants::Direction::Right : direction += 2; break;
		case Constants::Direction::RightDown : direction += 3; break;
		case Constants::Direction::Down : direction += 4; break;
		case Constants::Direction::LeftDown : direction += 5; break;
		case Constants::Direction::Left : direction += 6; break;
		default : direction += 7;
		}
		if (direction > 7) direction -= 8;

		switch (direction) {
		case 0 :
			directionF = Lunia::XRated::Constants::DirectionF::Up;
			break;
		case 1 :
			directionF = Lunia::XRated::Constants::DirectionF::RightUp;
			break;
		case 2 :
			directionF = Lunia::XRated::Constants::DirectionF::Right;
			break;
		case 3 :
			directionF = Lunia::XRated::Constants::DirectionF::RightDown;
			break;
		case 4 :
			directionF = Lunia::XRated::Constants::DirectionF::Down;
			break;
		case 5 :
			directionF = Lunia::XRated::Constants::DirectionF::LeftDown;
			break;
		case 6 :
			directionF = Lunia::XRated::Constants::DirectionF::Left;
			break;
		default :
			directionF = Lunia::XRated::Constants::DirectionF::LeftUp;
		}

		directionVector = directionF * speed;
	}

	bool Step::Do(float dt, IGameStateDatabase* db)
	{
		if (pauseTime > 0) {
			//������ ���¶�� ������ �ð��� ������ ���� �ƹ��͵� ���� �ʴ´�.
			if (pauseTime - dt <= 0) {//���� ���´� Ǯ����.
				dt -= pauseTime;
				pauseTime = 0;
			} else {//������ ���� ����
				pauseTime -= dt;
				return false;
			}
		}

		if (owner->CheckState(Info::StateInfo::Type::ROOT) || owner->CheckState(Info::StateInfo::Type::FREEZE))	//��Ʈ�� �ɷ����� �������� ���Ѵ�.
			return false;

		float3 pos = owner->GetPosition();
		pos += directionVector * dt;

		bool bAbletoLocate;
		if (owner->CheckState(Info::StateInfo::Type::GHOST))//���ɻ��¸� �ɸ��ʹ� ���.
			bAbletoLocate = db->IsAbletoLocateByGhost(pos, owner->GetRadius());
		else
			bAbletoLocate = db->IsAbletoLocate(owner->GetSerial(), pos, owner->GetRadius(), owner);

		if (bAbletoLocate)
			owner->SetPosition(pos);

		if ( (!owner->IsCollide() && !bAbletoLocate) || (owner->IsCollide() && bAbletoLocate) ) {
			owner->ChangeCollisionState();
			db->CollisionStateChanged(owner->GetSerial(), owner->IsCollide(), owner->GetPosition());
		}
		return false;
	}


	void Taunt::Initialize(IGameStateDatabase* db)
	{
		CharacterData& data = owner->GetCharacterData();
		std::vector<Actor*> l;
		db->GetAreaObject(data.BaseObject.Position, range, Constants::AllianceType::ExTeam, data.Team, owner, l);
		std::vector<Actor*>::iterator i = l.begin(), end = l.end();

		for (; i != end ; ++i) {
			//���� ���̰ų�, npc �� �ƴ϶�� �н�
			if ( (*i)->GetType() == Constants::ObjectType::NonPlayer) {
				if ( db->GetRandomFloat() <= probability )
					((NonPlayer*)(*i))->Taunted(owner->GetSerial(), duration);
			}
		}
	}


	void Weaken::Initialize(IGameStateDatabase*)
	{
		owner->AddAcMul(-lowerRate);
	}
	
	void Weaken::Destroy()
	{
		owner->RemoveAcMul(-lowerRate);
	}

	void IgnoreState::Initialize(IGameStateDatabase*)
	{		
		owner->GetIgnoreState().AddCategory(ignoreCategory);
	}

	void IgnoreState::Destroy()
	{		
		owner->GetIgnoreState().RemoveCategory(ignoreCategory);
	}

	//void ConvertDamageToHpMp::Do(IGameStateDatabase* db)
	//{
	//	Actor* targetActor = (affectTarget == Other ? db->RetrieveActor(otherActorSerial) : owner);

	//	if(targetActor == NULL)
	//		return;

	//	Lunia::XRated::CharacterData& data = targetActor->GetCharacterData();

	//	float max, current, amount = damage * rate;

	//	switch(recoveryType)
	//	{
	//	case	RecoveryType::Hp:
	//		current = owner->GetHP(max);

	//		owner->SetHP(current+amount, max);

	//		db->StatusChanged(data.BaseObject.GameObjectSerial, data.BaseObject.Position, data.BaseObject.Direction,
	//			data.Hp, data.Mp, data.BaseObject.GameObjectSerial, Info::HashTable::Object::itemHpPotion);
	//		break;
	//	case	RecoveryType::Mp:
	//		current = owner->GetMP(max);

 //           owner->SetMP(current+amount, max);

	//		db->StatusChanged(data.BaseObject.GameObjectSerial, data.BaseObject.Position, data.BaseObject.Direction,
	//			data.Hp, data.Mp, data.BaseObject.GameObjectSerial, Info::HashTable::Object::itemMpPotion);
	//		break;
	//	}

	//	otherActorSerial = 0;
	//}

	//void ConvertDamageToHpMp::SetParamValue(XRated::Serial otherActorSerial, float damage)
	//{
	//	this->otherActorSerial = otherActorSerial;
	//	this->damage = damage;
	//}

	void DmgToHpMp::Initialize(IGameStateDatabase*)
	{
		if(convertTo == ConvertTo::Hp)
			owner->AddDamageToHpRate(rate, probability);
		else if(convertTo == ConvertTo::Mp)
			owner->AddDamageToMpRate(rate, probability);
	}
	
	void DmgToHpMp::Destroy()
	{
		if(convertTo == ConvertTo::Hp)
			owner->AddDamageToHpRate(-rate, probability);
		else if(convertTo == ConvertTo::Mp)
			owner->AddDamageToMpRate(-rate, probability);
	}

	//void DmgToHpMp::Do(IGameStateDatabase*)
	//{
	//	float maxValue;
	//	float currentValue;

	//	if(convertTo == ConverTo::Hp)
	//	{
	//		currentValue = owner->GetHP(maxValue);
	//		owner->SetHP(currentValue + damage*rate, maxValue);
	//	}
	//	else if(convertTo == ConverTo::Mp)
	//	{
	//		currentValue = owner->GetHP(maxValue);
	//		owner->SetMP(currentValue + damage*rate, maxValue);
	//	}
	//}

	void AbsorbToHpMp::Initialize(IGameStateDatabase*)
	{
		if(convertTo == ConvertTo::Hp)
			owner->AddAbsorbToHpRate(rate, probability);
		else if(convertTo == ConvertTo::Mp)
			owner->AddAbsorbToMpRate(rate, probability);
	}
	
	void AbsorbToHpMp::Destroy()
	{
		if(convertTo == ConvertTo::Hp)
			owner->AddAbsorbToHpRate(-rate, probability);
		else if(convertTo == ConvertTo::Mp)
			owner->AddAbsorbToMpRate(-rate, probability);
	}


	void InventFame::Initialize(IGameStateDatabase* /*db*/)
	{
	}

	void InventFame::Destroy()
	{
	}

	//void AbsorbToHpMp::Do(IGameStateDatabase*)
	//{
	//	float maxValue;
	//	float currentValue;

	//	if(convertTo == ConverTo::Hp)
	//	{
	//		currentValue = owner->GetHP(maxValue);
	//		owner->SetHP(currentValue + damage*rate, maxValue);
	//	}
	//	else if(convertTo == ConverTo::Mp)
	//	{
	//		currentValue = owner->GetHP(maxValue);
	//		owner->SetMP(currentValue + damage*rate, maxValue);
	//	}
	//}

	void GoldGain::Initialize(IGameStateDatabase* db) {
		db->GoldGained(owner, Constants::GoldRewardAcquiredType::InstantCoin, money);
	}

}	}	}	}
