#include "StateAddBuff.h"
#include "StateAreaHeal.h"
#include "StateAttack.h"
#include "StateBlind.h"
#include "StateCampfire.h"
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
#include "StateDown.h"
#include "StateFreeze.h"
#include "StateHate.h"
#include "StateIgnoreDmg.h"
#include "StateIgnoreStun.h"
#include "StateImmune.h"
#include "StateIncLife.h"
#include "StateIncStat.h"
#include "StateIncSkillLevel.h"
#include "StateInvincible.h"
#include "StateInvisible.h"
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
#include "StateRevive.h"
#include "StateRoot.h"
#include "StateSelfHeal.h"
#include "StateSleep.h"
#include "StateSlow.h"
#include "StateSpreadHate.h"
#include "StateStep.h"
#include "StateTaunt.h"
#include "StateWeaken.h"
#include "StateIgnoreState.h"
#include "StateCondition.h"
#include "StateIncrease.h"
#include "StateIgnoreCriticalHit.h"
#include "StateBlockUsingSkill.h"
#include "StateResetSkillGroupCooldown.h"
#include "StateDecreaseSkillCooldown.h"
#include "StateDmgToHpMp.h"
#include "StateAbsorbToHpMp.h"
#include "StateInventFame.h"
#include "StateGoldGain.h"
#include "States.h"
#include <iostream>


#include <Core/Utils/StringUtil.h>
#include "../Object/NonPlayer.h"
#include "../Object/Projectile.h"
#include <Info/Info/Database.h>


using namespace Lunia::StringUtil;
using namespace Lunia::XRated::Database;

namespace Lunia { namespace XRated {	namespace Logic { namespace Stat {

	
	/* optional state �� statebundle�� �߰� �Ϸ��� ����.
	template<class OptionalState> IState* CreateStateImpl(Info::StateInfo::Type type, Object* owner, StateParams* params,
										std::vector<OptionalState>* optionalStates, Lunia::XRated::Serial creator);
	template<class OptionalState> IState* CreateStateImpl(Info::StateInfo::Type type, Actor* owner, StateParams* params,
										std::vector<OptionalState>* optionalStates, Lunia::XRated::Serial creator);
	template<class OptionalState> IState* CreateStateImpl(Info::StateInfo::Type type, Player* owner, StateParams* params,
										std::vector<OptionalState>* optionalStates, Lunia::XRated::Serial creator);


	*/
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// CreateState function for Object	////////////////////////////////////////////////////////////////////////////////////////
	/* optional state �� statebundle�� �߰� �Ϸ��� ����.
	template<> IState* CreateState(Info::StateInfo::Type type, Object* owner, StateParams* params,
										std::vector<Database::Info::ActionInfo::State>* optionalStates, Lunia::XRated::Serial creator)
	{
		return CreateStateImpl(type, owner, params, optionalStates, creator);
	}
	template<> IState* CreateState(Info::StateInfo::Type type, Object* owner, StateParams* params,
		std::vector<Database::Info::StateBundleInfo::State>* optionalStates, Lunia::XRated::Serial creator)
	{
		return CreateStateImpl(type, owner, params, optionalStates, creator);
	}
	template<class OptionalState> IState* CreateStateImpl(Info::StateInfo::Type type, Object* owner, StateParams* params,
										std::vector<OptionalState>* optionalStates, Lunia::XRated::Serial creator)
	*/
	template<> 
	IState* CreateState(Info::StateInfo::Type type, Object* owner, StateParams* params,
										std::vector<Database::Info::ActionInfo::State>* optionalStates, Lunia::XRated::Serial creator)	
	{
		StateParams::iterator iter;

		switch ( type ) {
		case Info::StateInfo::Type::STUN :
			return new Stun(owner);
		case Info::StateInfo::Type::DESTROYENEMY :
			{
				float radius=0;
				Constants::ObjectType type = Constants::ObjectType::NonPlayer;
				DestroyEnemy::AreaType areaType = DestroyEnemy::AreaType::ONE;

				iter = (*params).find(L"targetType");
				if ( iter != (*params).end() )
					type = (Constants::ObjectType)To<int>((*iter).second.c_str());

				std::wstring param = (*params)[L"type"];
				if (param == L"ONE")
					areaType = DestroyEnemy::AreaType::ONE;
				else if (param == L"RANGE") {
					areaType = DestroyEnemy::AreaType::RANGE;
					param = (*params)[L"radius"];
					radius = To<float>(param);
				} else if (param == L"SCREEN")
					areaType = DestroyEnemy::AreaType::SCREEN;
				else if (param == L"ALLINGAME")
					areaType = DestroyEnemy::AreaType::ALLINGAME;
				else {
					Logger::GetInstance().Exception(L"[CreateState] Unknown type in DESTROYENEMY state.");
				}
				return new DestroyEnemy(owner, type, areaType, radius);
			}
		case Info::StateInfo::Type::DETECTINVISIBLE :
			{
				float range = 0;
				iter = (*params).find(L"range");
				if ( iter != (*params).end() )
					range = To<float>((*iter).second.c_str());

				return new DetectInvisible(owner, range);
			}
		case Info::StateInfo::Type::FLIED :
			return new Flied(owner);
		case Info::StateInfo::Type::INVINCIBLE :
			return new Invincible(owner, To<float>((*params)[L"duration"].c_str()));
		case Info::StateInfo::Type::MAKESTUN :
			return new MakeStun(owner, To<int>((*params)[L"area"].c_str()) ? true : false, To<float>((*params)[L"range"].c_str()), To<float>((*params)[L"stunLength"].c_str()) );
			/*
		case Info::StateInfo::Type::IGNORESTATE:
			{
				iter = (*params).find(L"category");
				if( iter != (*params).end() ) {
					return new IgnoreState(owner, To<uint32>( (*params)[L"category"].c_str() ) );
				}
			}
			break;
			*/
			
		}

		Logger::GetInstance().Exception("[Stat::CreateState<Object>] Unknown state [{0}].", (int)type);
		return NULL;
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// CreateState function for Projectile	////////////////////////////////////////////////////////////////////////////////////
	IState* CreateState(Info::StateInfo::Type type, Projectile* projectile, Actor* target, StateParams* params, Lunia::XRated::Serial /*creator*/)
	{
		switch(type) {
		case Info::StateInfo::Type::AREAHEAL :
			{
				float range = To<float>((*params)[L"range"]);
				float regenAmount =To<float>((*params)[L"regenAmount"]);
				AreaHeal::FillType ft = AreaHeal::FillType::Hp;
				Constants::AllianceType type= Constants::AllianceType::Team;
				if ( (*params).find(L"fillType") != (*params).end() )
					ft = (AreaHeal::FillType)StringUtil::To<int>((*params)[L"fillType"]);
				if ( (*params).find(L"allianceType") != (*params).end() )
					type = (Constants::AllianceType)StringUtil::To<int>((*params)[L"allianceType"]);

				return new AreaHeal(target, range, regenAmount, ft, type);
			}
		case Info::StateInfo::Type::ATTACK :
			{
				std::wstring param;
				StatConstants::AttackEffectType::Type effectType = StatConstants::AttackEffectType::DOWN;
				Attack::AttackDir attackDirection;

				param = (*params)[L"damageType"];
				Constants::DamageType dmgType = Constants::DamageType::PHYSICAL;
				if ( !param.empty() )
					dmgType = Constants::DamageType( To<int>(param) );

				param = (*params)[L"effectType"];
				if (param == L"FAINT")
					effectType = StatConstants::AttackEffectType::FAINT;
				else if (param == L"AIRSTUN")
					effectType = StatConstants::AttackEffectType::AIRSTUN;
				else if (param == L"DOWN")
					effectType = StatConstants::AttackEffectType::DOWN;
				else if (param == L"PHOENIXDOWN")
					effectType = StatConstants::AttackEffectType::PHOENIXDOWN;
				else if (param == L"PHOENIXDOWN2")  //@ORACLE 01JAN2020 13:35 EST
					effectType = StatConstants::AttackEffectType::PHOENIXDOWN2;
				 else if (param == L"CENTERMOVE")  //@ORACLE 01JAN2020 19:50 EST
					effectType = StatConstants::AttackEffectType::CENTERMOVE;
				else if (param == L"STUN")
					effectType = StatConstants::AttackEffectType::STUN;
				else if (param == L"STEPBACK")
					effectType = StatConstants::AttackEffectType::STEPBACK;
				else if (param == L"STANDFAINT")
					effectType = StatConstants::AttackEffectType::STANDFAINT;
				else if (param == L"NOSTUN")
					effectType = StatConstants::AttackEffectType::NOSTUN;
				else 
					Logger::GetInstance().Info(L"[Stat::CreateState] Unknown Attack state type [{0}] in ATTACK.", param.c_str());
				

				std::wstring strAttackDir = (*params)[L"attackDirection"];
				if (strAttackDir == L"L")
					attackDirection = Attack::AttackDir::L;
				else if (strAttackDir == L"F")
					attackDirection = Attack::AttackDir::F;
				else if (strAttackDir == L"R")
					attackDirection = Attack::AttackDir::R;
				else if (strAttackDir == L"B")
					attackDirection = Attack::AttackDir::B;
				else
					attackDirection = Attack::AttackDir::F;

				std::wstring strDmgTo = (*params)[L"DmgTo"];
				Constants::AllianceType dmgTo = Constants::AllianceType::ExTeam;
				if (strDmgTo == L"ALLOBJECTS")
					dmgTo = Constants::AllianceType::AnyObjects;
				else if (strDmgTo == L"TEAM")
					dmgTo = Constants::AllianceType::Team;
				else if (strDmgTo == L"EXTEAM")
					dmgTo = Constants::AllianceType::ExTeam;
				else if (strDmgTo == L"ALLIANCE")
					dmgTo = Constants::AllianceType::Alliance;
				else if (strDmgTo == L"EXALLIANCE")
					dmgTo = Constants::AllianceType::ExAlliance;

				bool bAbsoluteDmg;
				float min=0, max=0;
				if ( ( *params ).find( L"maxDmg" ) != ( *params ).end() && !( *params )[ L"maxDmg" ].empty() ) 
				{
					max = To< float >( ( *params )[ L"maxDmg" ] );
					min = max * To< float >( ( *params )[ L"minDmgRate" ] );
					bAbsoluteDmg = true;
				} 
				else 
				{
					bAbsoluteDmg = false;

					//projectile->GetDamage(min, max);
					if ( ( *params ).find( L"relativeDmg") != ( *params ).end() && !( *params )[ L"relativeDmg" ].empty() ) 
					{
						min = To< float >( ( *params )[ L"relativeDmg" ] );
						//float relativeDmg = To<float>((*params)[L"relativeDmg"]);
						//min *= relativeDmg;
						//max *= relativeDmg;
					} 
					else
					{
						min = 1;
					}
				}

				float3 startPos(To<float>((*params)[L"x"]), 0, To<float>((*params)[L"z"]));

				int targetType;
				if ( (*params).find(L"targetType") == (*params).end() ) 
					targetType = 6;
				 else 
					targetType = To<int>((*params)[L"targetType"]);
				

				float heightMax = 15.0f;
				if ( (*params).find(L"heightMax") != (*params).end() )
					heightMax = To<float>( (*params)[L"heightMax"] );


				//Backstab info
				Attack::BackstabInfo backstabInfo;

				param = (*params)[L"type"];
				if (param == L"CIRCLE")
					return new AttackCircle(projectile, effectType, targetType, dmgType, dmgTo, startPos, To<float>((*params)[L"range"]),
											bAbsoluteDmg, min, max, To<float>((*params)[L"knockBack"]), To<float>((*params)[L"airKnockBack"]),
											To<float>((*params)[L"stunLength"]), attackDirection, To<float>((*params)[L"airMakeFly"]),
											(effectType == StatConstants::AttackEffectType::AIRSTUN ? To<float>((*params)[L"makeFly"]):0), To<float>((*params)[L"stopDelay"]), heightMax, backstabInfo);
				else if (param == L"RECTANGLE")
					return new AttackRectangle(projectile, effectType, targetType, dmgType, dmgTo, startPos, To<float>((*params)[L"range"]),
											bAbsoluteDmg, min, max, To<float>((*params)[L"knockBack"]), To<float>((*params)[L"airKnockBack"]),
											To<float>((*params)[L"stunLength"]), attackDirection, To<float>((*params)[L"width"]), To<float>((*params)[L"airMakeFly"]),
											(effectType == StatConstants::AttackEffectType::AIRSTUN ? To<float>((*params)[L"makeFly"]):0), To<float>((*params)[L"stopDelay"]), heightMax, backstabInfo);
				else if (param == L"FANSHAPE")
					return new AttackFanshape(projectile, effectType, targetType, dmgType, dmgTo, startPos, To<float>((*params)[L"range"]),
											bAbsoluteDmg, min, max, To<float>((*params)[L"knockBack"]), To<float>((*params)[L"airKnockBack"]),
											To<float>((*params)[L"stunLength"]), attackDirection, To<float>((*params)[L"startAngle"]),
											To<float>((*params)[L"endAngle"]), To<float>((*params)[L"airMakeFly"]),
											(effectType == StatConstants::AttackEffectType::AIRSTUN ? To<float>((*params)[L"makeFly"]):0), To<float>((*params)[L"stopDelay"]), heightMax, backstabInfo);
				else if (param == L"SEGMENT")
					return new AttackSegment(projectile, effectType, targetType, dmgType, dmgTo, startPos, To<float>((*params)[L"range"]),
											bAbsoluteDmg, min, max, To<float>((*params)[L"knockBack"]), To<float>((*params)[L"airKnockBack"]),
											To<float>((*params)[L"stunLength"]), attackDirection, To<float>((*params)[L"airMakeFly"]),
											(effectType == StatConstants::AttackEffectType::AIRSTUN ? To<float>((*params)[L"makeFly"]):0), To<float>((*params)[L"stopDelay"]), heightMax, backstabInfo);
				else {
					Logger::GetInstance().Info(L"[Stat::CreateState] Unknown Attack range type [{0}].", param.c_str());
					return NULL;
				}
			}
			break;
		case Info::StateInfo::CHANGEACTION :
			{
				std::wstring param;
				uint32 actionHash=0, targetHash=0;
				Database::Info::StateInfo::RangeType type = Database::Info::StateInfo::RangeType::Self;
				Constants::AllianceType applyTo = Constants::AllianceType::ExTeam;
				bool bForce = true;
				float range = 1.0f;
				if ( (*params).find(L"type") != (*params).end() ) {
					param = (*params)[L"type"];
					if ( param == L"CIRCLE" )
						type = Database::Info::StateInfo::RangeType::Circle;
					else if ( param == L"SELF" )
						type = Database::Info::StateInfo::RangeType::Self;
					//type = (Database::Info::StateInfo::RangeType)StringUtil::To<int>(param);
				}
				if ( (*params).find(L"actionName") != (*params).end() ) {
					param = (*params)[L"actionName"];
					actionHash = StringUtil::Hash(param.c_str());
				}
				if ( (*params).find(L"targetID") != (*params).end() ) {
					param = (*params)[L"targetID"];
					targetHash = StringUtil::Hash(param.c_str());
				}
				if ( (*params).find(L"applyTo") != (*params).end() ) {
					param = (*params)[L"applyTo"];
					if (param == L"ALLOBJECTS")
						applyTo = Constants::AllianceType::AnyObjects;
					else if (param == L"TEAM")
						applyTo = Constants::AllianceType::Team;
					else if (param == L"EXTEAM")
						applyTo = Constants::AllianceType::ExTeam;
				}
				if ( (*params).find(L"bForce") != (*params).end() ) {
					param = (*params)[L"bForce"];
					if (param == L"false")
						bForce = false;
					else
						bForce = true;
				}
				if ( (*params).find(L"range") != (*params).end() ) {
					param = (*params)[L"range"];
					range = StringUtil::To<float>(param);
				}
				return new ChangeAction(target, type, applyTo, range, actionHash, targetHash, bForce);
			}
		case Info::StateInfo::CREATEPROJECTILE :
			{
				std::wstring nameStr = (*params)[L"name"];
				uint32 name = StringUtil::Hash(nameStr.c_str());
				float2 pos = To<float2>((*params)[L"pos"]);
				uint16 angle = To<uint16>((*params)[L"angle"]), type = To<uint16>((*params)[L"type"]), toMyTarget = To<uint16>((*params)[L"toMyTarget"]);
				float min = To<float>((*params)[L"min"]), max = To<float>((*params)[L"max"]);
				float time = To<float>((*params)[L"createTime"]);
	#ifdef _SINGLE
				if (type != 1)
					Lunia_INFO(( "[CreateState] CREATEPROJECTILE state must have type 1." ));
	#endif
				type = 1;

				return new CreateProjectile(projectile, name, pos, angle, type, min, max, toMyTarget, time);
			}
		case  Info::StateInfo::Type::DAMAGE :
			{
				float min=0, max=0;
				if ( (*params).find(L"maxDmg") != (*params).end() && !(*params)[L"maxDmg"].empty() ) {
					max = To<float>( (*params)[L"maxDmg"] );
					min = max * To<float>( (*params)[L"minDmgRate"] );
				} else {
					if ( projectile )
						projectile->GetDamage(min, max);
				}

				Stat::StatConstants::AttackEffectType::Type effectType = StatConstants::AttackEffectType::DOWN;
				std::wstring param = (*params)[L"effectType"];
				if (param == L"STUN")
					effectType = StatConstants::AttackEffectType::STUN;
				else if (param == L"AIRSTUN")
					effectType = StatConstants::AttackEffectType::AIRSTUN;
				else if (param == L"DOWN")
					effectType = StatConstants::AttackEffectType::DOWN;
				else if (param == L"PHOENIXDOWN")
					effectType = StatConstants::AttackEffectType::PHOENIXDOWN;
				else if (param == L"PHOENIXDOWN2"){ //@ORACLE 01JAN2020 01:35 EST
					effectType = StatConstants::AttackEffectType::PHOENIXDOWN2;
				} else if (param == L"CENTERMOVE"){ //@ORACLE 01JAN2020 19:52 EST
					effectType = StatConstants::AttackEffectType::CENTERMOVE;
				}
				else if (param == L"FAINT")
					effectType = StatConstants::AttackEffectType::FAINT;
				else if (param == L"STEPBACK")
					effectType = StatConstants::AttackEffectType::STEPBACK;
				else if (param == L"STANDFAINT")
					effectType = StatConstants::AttackEffectType::STANDFAINT;
				else if (param == L"NOSTUN")
					effectType = StatConstants::AttackEffectType::NOSTUN;
				else {
					Logger::GetInstance().Info(L"[Stat::CreateState] Unknown Attack state type [{0}] in DAMAGE.", param.c_str());
				}

				param = (*params)[L"damageType"];
				Constants::DamageType dmgType = Constants::DamageType::PHYSICAL;
				if ( !param.empty() )
					dmgType = (Constants::DamageType)( To<int>(param) );

				int targetType;
				if ( (*params).find(L"targetType") == (*params).end() ) {
					targetType = 6;//�⺻�� ���ִ� �༮�� ���߿� ���ִ³༮���� �����Ҽ� �ִ�.
				} else {
					targetType = To<int>((*params)[L"targetType"]);
				}

				return new Damage(target, min, max, 0, dmgType, effectType, targetType, To<float>((*params)[L"knockBack"])
									, To<float>((*params)[L"makeFly"]), To<float>((*params)[L"stunLength"]), To<float>((*params)[L"airMakeFly"])
									, To<float>((*params)[L"airKnockBack"]));
			}
		case Info::StateInfo::Type::ROOT :
			return new Root(target);
		case Info::StateInfo::Type::IGNORESTATE:
			{
				StateParams::iterator iter = (*params).find(L"category");
				if( iter != (*params).end() ) {
					return new IgnoreState(target, To<uint32>( (*params)[L"category"].c_str() ) );
				}
			}
			break;
		default :
			{
				IState* returnValue = CreateState(type, (Object*)target, params, static_cast<std::vector<Database::Info::ActionInfo::State>* >(NULL) );
				if (returnValue) return returnValue;
			}
		}
		Logger::GetInstance().Exception("[Stat::CreateState] Unknown state [{0}].", (int)type);
		throw; // supressing a warning lol
	}



	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// CreateState function for Actor	////////////////////////////////////////////////////////////////////////////////////////
	/* optional state �� statebundle�� �߰� �Ϸ��� ����.
	template<> IState* CreateState(Info::StateInfo::Type type, Actor* owner, StateParams* params,
										std::vector<Database::Info::ActionInfo::State>* optionalStates, Lunia::XRated::Serial creator)
	{
		return CreateStateImpl(type, owner, params, optionalStates, creator);
	}
	template<> IState* CreateState(Info::StateInfo::Type type, Actor* owner, StateParams* params,
		std::vector<Database::Info::StateBundleInfo::State>* optionalStates, Lunia::XRated::Serial creator)
	{
		return CreateStateImpl(type, owner, params, optionalStates, creator);
	}
	template<class OptionalState> IState* CreateStateImpl(Info::StateInfo::Type type, Actor* owner, StateParams* params,
										std::vector<OptionalState>* optionalStates, Lunia::XRated::Serial creator)
	*/
	template<> IState* CreateState(Info::StateInfo::Type type, Actor* owner, StateParams* params,
										std::vector<Database::Info::ActionInfo::State>* optionalStates, Lunia::XRated::Serial creator)
	{
		switch(type) {
		case Info::StateInfo::Type::ADDBUFFWHENHIT :
			{
				float prob = To<float>((*params)[L"probability"]);
				uint32 hash= StringUtil::Hash((*params)[L"stateBundleName"].c_str());
				bool bSelf = To<bool>((*params)[L"bSelf"].c_str());
				return new AddBuffWhenHit(owner, prob, hash, bSelf);
			}
		case Info::StateInfo::Type::ADDBUFFWHENHITTED :
			{
				float prob = To<float>((*params)[L"probability"]);
				uint32 hash= StringUtil::Hash((*params)[L"stateBundleName"].c_str());
				bool bSelf = To<bool>((*params)[L"bSelf"].c_str());
				return new AddBuffWhenHitted(owner, prob, hash, bSelf);
			}
		case Info::StateInfo::Type::AREAHEAL :
			{
				float range = To<float>((*params)[L"range"]);
				float regenAmount =To<float>((*params)[L"regenAmount"]);
				AreaHeal::FillType ft = AreaHeal::FillType::Hp;
				Constants::AllianceType type= Constants::AllianceType::Team;
				if ( (*params).find(L"fillType") != (*params).end() )
					ft = (AreaHeal::FillType)StringUtil::To<int>((*params)[L"fillType"]);
				if ( (*params).find(L"allianceType") != (*params).end() )
					type = (Constants::AllianceType)StringUtil::To<int>((*params)[L"allianceType"]);

				return new AreaHeal(owner, range, regenAmount, ft, type);
			}
		case Info::StateInfo::Type::ATTACK :
			{
				std::wstring param;
				StatConstants::AttackEffectType::Type effectType = StatConstants::AttackEffectType::DOWN;
				Attack::AttackDir attackDirection;

				param = (*params)[L"damageType"];
				Constants::DamageType dmgType = Constants::DamageType::PHYSICAL;
				if ( !param.empty() )
					dmgType = Constants::DamageType( To<int>(param) );

				param = (*params)[L"effectType"];
				if (param == L"FAINT")
					effectType = StatConstants::AttackEffectType::FAINT;
				else if (param == L"AIRSTUN")
					effectType = StatConstants::AttackEffectType::AIRSTUN;
				else if (param == L"DOWN")
					effectType = StatConstants::AttackEffectType::DOWN;
				else if (param == L"PHOENIXDOWN")
					effectType = StatConstants::AttackEffectType::PHOENIXDOWN;
				else if (param == L"PHOENIXDOWN2"){ //@ORALCE 01JAN2020 13:35 EST
					effectType = StatConstants::AttackEffectType::PHOENIXDOWN2;
				} else if (param == L"CENTERMOVE"){ //@ORALCE 01JAN2020 19:52 EST
					effectType = StatConstants::AttackEffectType::CENTERMOVE;
				}
				else if (param == L"STUN")
					effectType = StatConstants::AttackEffectType::STUN;
				else if (param == L"STEPBACK")
					effectType = StatConstants::AttackEffectType::STEPBACK;
				else if (param == L"STANDFAINT")
					effectType = StatConstants::AttackEffectType::STANDFAINT;
				else if (param == L"NOSTUN")
					effectType = StatConstants::AttackEffectType::NOSTUN;
				else {
					Logger::GetInstance().Info(L"[Stat::CreateState] Unknown Attack state type [{0}].", param.c_str());
					//return NULL;
				}

				std::wstring strAttackDir = params->find(L"attackDirection") != params->end() ?  (*params)[L"attackDirection"] : L"F";
				if (strAttackDir == L"L")
					attackDirection = Attack::AttackDir::L;
				else if (strAttackDir == L"F")
					attackDirection = Attack::AttackDir::F;
				else if (strAttackDir == L"R")
					attackDirection = Attack::AttackDir::R;
				else if (strAttackDir == L"B")
					attackDirection = Attack::AttackDir::B;
				else
					attackDirection = Attack::AttackDir::F;

				std::wstring strDmgTo = (*params)[L"DmgTo"];
				Constants::AllianceType dmgTo = Constants::AllianceType::ExTeam;
				if (strDmgTo == L"ALLOBJECTS")
					dmgTo = Constants::AllianceType::AnyObjects;
				else if (strDmgTo == L"TEAM")
					dmgTo = Constants::AllianceType::Team;
				else if (strDmgTo == L"EXTEAM")
					dmgTo = Constants::AllianceType::ExTeam;
				else if (strDmgTo == L"ALLIANCE")
					dmgTo = Constants::AllianceType::Alliance;
				else if (strDmgTo == L"EXALLIANCE")
					dmgTo = Constants::AllianceType::ExAlliance;

				float min, max;
				bool bAbsoluteDmg;
				if ( (*params).find(L"maxDmg") != (*params).end() && !(*params)[L"maxDmg"].empty() ) {
					max = To<float>( (*params)[L"maxDmg"] );
					min = max * To<float>( (*params)[L"minDmgRate"] );
					bAbsoluteDmg = true;
				} else {
					min = max = 0;
					bAbsoluteDmg = false;
					
					//owner->GetDamage(min, max);

					if ( (*params).find(L"relativeDmg") != (*params).end() && !(*params)[L"relativeDmg"].empty() ) {
						min = To<float>((*params)[L"relativeDmg"]);
						//float relativeDmg = To<float>((*params)[L"relativeDmg"]);
						//min *= relativeDmg;
						//max *= relativeDmg;
					} else
						min = 1;
				}

				float3 startPos(To<float>((*params)[L"x"]), 0, To<float>((*params)[L"z"]));

				int targetType;
				if ( (*params).find(L"targetType") == (*params).end() ) {
					targetType = 6;//�⺻�� ���ִ� �༮�� ���߿� ���ִ³༮���� �����Ҽ� �ִ�.
				} else {
					targetType = To<int>((*params)[L"targetType"]);
				}

				float heightMax = 15.0f;
				if ( (*params).find(L"heightMax") != (*params).end() )
					heightMax = To<float>( (*params)[L"heightMax"] );

				//Backstab info
				Attack::BackstabInfo backstab;
				if ( (*params).find(L"bBackstab") != (*params).end() ) {
					backstab.bBackstab = To<bool>((*params)[L"bBackstab"].c_str());
					backstab.bStun = To<bool>((*params)[L"bBackstabStun"].c_str());
					backstab.backstabDmg = To<float>((*params)[L"backstabDmg"]);
					backstab.stunLength = To<float>((*params)[L"backstabStunLength"]);
				}


				param = (*params)[L"type"];
				if (param == L"CIRCLE") {
					return new AttackCircle(owner, effectType, targetType, dmgType, dmgTo, startPos, To<float>((*params)[L"range"]), bAbsoluteDmg, min, max,
											To<float>((*params)[L"knockBack"]), To<float>((*params)[L"airKnockBack"]),
											To<float>((*params)[L"stunLength"]), attackDirection, To<float>((*params)[L"airMakeFly"]),
											(effectType == StatConstants::AttackEffectType::AIRSTUN ? To<float>((*params)[L"makeFly"]):0),
											To<float>((*params)[L"stopDelay"]), heightMax, backstab);
				} else if (param == L"RECTANGLE") {
					return new AttackRectangle(owner, effectType, targetType, dmgType, dmgTo, startPos, To<float>((*params)[L"range"]), bAbsoluteDmg, min, max,
											To<float>((*params)[L"knockBack"]), To<float>((*params)[L"airKnockBack"]),
											To<float>((*params)[L"stunLength"]), attackDirection, To<float>((*params)[L"width"]),
											To<float>((*params)[L"airMakeFly"]),
											(effectType == StatConstants::AttackEffectType::AIRSTUN ? To<float>((*params)[L"makeFly"]):0),
											To<float>((*params)[L"stopDelay"]), heightMax, backstab);
				} else if (param == L"FANSHAPE") {
					return new AttackFanshape(owner, effectType, targetType, dmgType, dmgTo, startPos, To<float>((*params)[L"range"]), bAbsoluteDmg, min, max,
											To<float>((*params)[L"knockBack"]), To<float>((*params)[L"airKnockBack"]),
											To<float>((*params)[L"stunLength"]), attackDirection, To<float>((*params)[L"startAngle"]),
											To<float>((*params)[L"endAngle"]), To<float>((*params)[L"airMakeFly"]),
											(effectType == StatConstants::AttackEffectType::AIRSTUN ? To<float>((*params)[L"makeFly"]):0),
											To<float>((*params)[L"stopDelay"]), heightMax, backstab);
				} else if (param == L"SEGMENT") {
					return new AttackSegment(owner, effectType, targetType, dmgType, dmgTo, startPos, To<float>((*params)[L"range"]), bAbsoluteDmg, min, max,
											To<float>((*params)[L"knockBack"]), To<float>((*params)[L"airKnockBack"]),
											To<float>((*params)[L"stunLength"]), attackDirection, To<float>((*params)[L"airMakeFly"]),
											(effectType == StatConstants::AttackEffectType::AIRSTUN ? To<float>((*params)[L"makeFly"]):0),
											To<float>((*params)[L"stopDelay"]), heightMax, backstab);
				} else {
					Logger::GetInstance().Info(L"[Stat::CreateState] Unknown Attack range type [{0}].", param.c_str());
					return NULL;
				}
			}
		case Info::StateInfo::Type::BLIND :
			return new Blind(owner);
		case Info::StateInfo::Type::BLOCKUSINGSKILL :
			return new BlockUsingSkill(owner);
		case Info::StateInfo::Type::CAMPFIRE :
			return new Campfire(owner);
		case Info::StateInfo::Type::CHANGEACTION :
			{
				std::wstring param;
				uint32 actionHash=0, targetHash=0;
				Database::Info::StateInfo::RangeType type = Database::Info::StateInfo::RangeType::Self;
				Constants::AllianceType applyTo = Constants::AllianceType::ExTeam;
				bool bForce = true;
				float range = 1.0f;
				if ( (*params).find(L"type") != (*params).end() ) {
					param = (*params)[L"type"];
					if ( param == L"CIRCLE" )
						type = Database::Info::StateInfo::RangeType::Circle;
					else if ( param == L"SELF" )
						type = Database::Info::StateInfo::RangeType::Self;
					//type = (Database::Info::StateInfo::RangeType)StringUtil::To<int>(param);
				}
				if ( (*params).find(L"actionName") != (*params).end() ) {
					param = (*params)[L"actionName"];
					actionHash = StringUtil::Hash(param.c_str());
				}
				if ( (*params).find(L"targetID") != (*params).end() ) {
					param = (*params)[L"targetID"];
					targetHash = StringUtil::Hash(param.c_str());
				}
				if ( (*params).find(L"applyTo") != (*params).end() ) {
					param = (*params)[L"applyTo"];
					if (param == L"ALLOBJECTS")
						applyTo = Constants::AllianceType::AnyObjects;
					else if (param == L"TEAM")
						applyTo = Constants::AllianceType::Team;
					else if (param == L"EXTEAM")
						applyTo = Constants::AllianceType::ExTeam;
				}
				if ( (*params).find(L"bForce") != (*params).end() ) {
					param = (*params)[L"bForce"];
					if (param == L"false")
						bForce = false;
					else
						bForce = true;
				}
				if ( (*params).find(L"range") != (*params).end() ) {
					param = (*params)[L"range"];
					range = StringUtil::To<float>(param);
				}
				return new ChangeAction(owner, type, applyTo, range, actionHash, targetHash, bForce);
			}
		case Info::StateInfo::Type::CHARGE :
			{
				std::wstring param = (*params)[L"value"];
				float amount = To<float>(param);
				Charge::Category category = Charge::Category::HP;
				Charge::ChargeType chargeType = Charge::ChargeType::RATE;

				param = (*params)[L"category"];
				if (param == L"HP")
					category = Charge::Category::HP;
				else if (param == L"MP")
					category = Charge::Category::MP;
				else if (param == L"DASH")
					category = Charge::Category::DASH;
				else {
					Logger::GetInstance().Info(L"[StateGlobalFunction::CreateState] Wrong category in CHARGE.");
				}
				param = (*params)[L"type"];
				if (param == L"RATE")
					chargeType = Charge::ChargeType::RATE;
				else if (param == L"AMOUNT")
					chargeType = Charge::ChargeType::AMOUNT;
				else {
					Logger::GetInstance().Info(L"[StateGlobalFunction::CreateState] Wrong chargeType in CHARGE.");
				}
				return new Charge(owner, category, chargeType, amount);
			}
		case Info::StateInfo::Type::CLEAR :
			{
				std::wstring param(L"Default");
				uint32 hash=0;
				if ( (*params).find(L"subCategoryName") != (*params).end() ) {
					param = (*params)[L"subCategoryName"];
				}
				hash = StringUtil::Hash(param.c_str());

				return new Clear(owner, To<float>((*params)[L"range"]), To<float>((*params)[L"debuffRate"]), To<int>((*params)[L"sort"]), hash );
			}
		case Info::StateInfo::CREATEPROJECTILE :
			{
				std::wstring nameStr = (*params)[L"name"];
				uint32 name = StringUtil::Hash(nameStr.c_str());
				float2 pos = To<float2>((*params)[L"pos"]);
				uint16 angle = To<uint16>((*params)[L"angle"]), type = To<uint16>((*params)[L"type"]), toMyTarget = To<uint16>((*params)[L"toMyTarget"]);
				float min = To<float>((*params)[L"min"]), max = To<float>((*params)[L"max"]);
				float time = To<float>((*params)[L"createTime"]);
	#ifdef _SINGLE
				if (type != 1)
					Logger::GetInstance().Info( "[CreateState] CREATEPROJECTILE state must have type 1." );
	#endif
				type = 1;

				return new CreateObjectProjectile(owner, name, pos, angle, type, min, max, toMyTarget, time);
			}
		case Info::StateInfo::Type::CREATENPC :
			if ( params->find( L"team" ) != params->end() )
			{
				return new CreateNPC(owner, To<uint32>((*params)[L"id"].c_str()), float2(To<float>((*params)[L"x"].c_str()), To<float>((*params)[L"y"].c_str())),
									To<int>((*params)[L"idleType"].c_str()), To<int>((*params)[L"transType"].c_str()),
									To<uint16>((*params)[L"defenceLocation"].c_str()), To<uint16>((*params)[L"path"].c_str()),
									To<uint16>((*params)[L"alliance"].c_str()), To<uint16>((*params)[L"team"].c_str()) );	
			}
			else
			{
				return new CreateNPC(owner, To<uint32>((*params)[L"id"].c_str()), float2(To<float>((*params)[L"x"].c_str()), To<float>((*params)[L"y"].c_str())),
									To<int>((*params)[L"idleType"].c_str()), To<int>((*params)[L"transType"].c_str()),
									To<uint16>((*params)[L"defenceLocation"].c_str()), To<uint16>((*params)[L"path"].c_str()),
									To<uint16>((*params)[L"alliance"].c_str()), 9999 );
			}
			
		case Info::StateInfo::Type::CREATEOBJECT :
			return new CreateObject(owner, To<uint32>((*params)[L"id"].c_str()), float2(To<float>((*params)[L"x"].c_str()), To<float>((*params)[L"y"].c_str())),
									To<int>((*params)[L"hp"].c_str()), To<float>((*params)[L"duration"].c_str()));
		case Info::StateInfo::Type::CRIALLINC :
			return new CriAllInc( owner, To<float>((*params)[L"amount"]) );
		case Info::StateInfo::Type::CRIATK :
			return new CriAtk( owner, To<float>((*params)[L"probability"]) );
		case Info::StateInfo::Type::CRIATKINC :
			return new CriAtkInc( owner, To<float>((*params)[L"amount"]) );
		case Info::StateInfo::Type::ELEMENTALATK:
			return new ElementalAtk( owner, Constants::StringToDmgType((*params)[L"element"]) );
		case Info::StateInfo::Type::CRIMAGIC :
			return new CriMagic(owner, To<float>((*params)[L"probability"]));
		case Info::StateInfo::Type::CRIMAGICINC :
			return new CriMagicInc( owner, To<float>((*params)[L"amount"]) );
		case Info::StateInfo::Type::CRIRUSH :
			return new CriRush(owner);
		case Info::StateInfo::Type::DEFENCE :
			return new Defence(owner, To<float>((*params)[L"probability"]), To<uint16>((*params)[L"type"]));
		case Info::StateInfo::Type::DESTROY :
			return new Destroy(owner);
		case Info::StateInfo::Type::DEVINESHD :
			{
				float duration = 0;
				if ( (*params).find(L"duration") != (*params).end() ) {
					duration = To<float>((*params)[L"duration"]);
				}
				return new DevineShd(owner, duration );
			}
		case Info::StateInfo::Type::DOT :
			{
				float duration = 0;
				float interval = 1.0f;
				if ( (*params).find(L"duration") != (*params).end() ) {
					duration = To<float>((*params)[L"duration"]);
				}
				if ( (*params).find(L"interval") != (*params).end() ) {
					interval = StringUtil::To<float>((*params)[L"interval"]);
				}

				return new Dot(owner, duration, To<float>((*params)[L"dps"]), interval, (Constants::DamageType)(To<int>((*params)[L"type"])));
			}
		case Info::StateInfo::Type::DOTMP :
			{
				float duration = 0;
				float interval = 1.0f;
				if ( (*params).find(L"duration") != (*params).end() ) {
					duration = To<float>((*params)[L"duration"]);
				}
				if ( (*params).find(L"interval") != (*params).end() ) {
					interval = StringUtil::To<float>((*params)[L"interval"]);
				}

				return new DotMP(owner, duration, To<float>((*params)[L"dps"]), interval);
			}
		case Info::StateInfo::Type::CHOKE :
			{
				float duration = 0;
				float interval = 1.0f;
				if ( (*params).find(L"duration") != (*params).end() ) {
					duration = To<float>((*params)[L"duration"]);
				}
				if ( (*params).find(L"interval") != (*params).end() ) {
					interval = StringUtil::To<float>((*params)[L"interval"]);
				}

				return new Choke(owner, duration, To<float>((*params)[L"dps"]), interval, (Constants::DamageType)(To<int>((*params)[L"type"])));
			}
		case Info::StateInfo::Type::DOWN :
			return new Down(owner);
		case Info::StateInfo::Type::FREEZE :
			return new Freeze(owner);
		case Info::StateInfo::Type::GHOST :
			return new Ghost(owner);
		case Info::StateInfo::Type::HATE :
			return new Hate(owner);
		case Database::Info::StateInfo::Type::INCDMG :
			{
				float value = 0.0f;
				XRated::Constants::DamageType damageType = Constants::DamageType::PHYSICAL;
				
				StateParams::iterator itr = (*params).find(L"damageType");
				if(itr != (*params).end())
					damageType = (Constants::DamageType)(To<int>(itr->second.c_str()));

				itr = (*params).find(L"rate");
				if(itr != (*params).end())
					value = To<float>(itr->second.c_str());

				return new IncDmg(owner, damageType, value);
			}
			break;
		case Database::Info::StateInfo::Type::INCHEALAMOUNT :
			{
				float value = 0.0f;

				StateParams::iterator itr = (*params).find(L"rate");
				if(itr != (*params).end())
					value = To<float>(itr->second.c_str());

				return new IncHealAmount(owner, value);
			}
			break;
		case Info::StateInfo::Type::IGNOREDMG :
			return new IgnoreDmg(owner, To<float>((*params)[L"probability"]) );
		case Info::StateInfo::Type::IGNORESTUN :
			{
				uint8 useState = 0; //0 : PvP������ �Ұ���.. 1 : ��� ������������ ����..
				StateParams::iterator itr = (*params).find(L"useState");
				if(itr != (*params).end())
					useState = To<uint8>(itr->second.c_str());

				return new IgnoreStun(owner, To<uint32>((*params)[L"flag"]), To<bool>((*params)[L"ignore"]), useState );
			}
			break;
		case Info::StateInfo::Type::IGNORECRITICALHIT :
			return new IgnoreCriticalHit(owner, To<float>((*params).find(L"probability")->second) );
		case Info::StateInfo::Type::IMMUNE :
			return new Immune(owner, To<uint16>((*params)[L"type"]) );
		case Info::StateInfo::Type::INCLIFE :
			return new IncLife(owner, To<uint16>((*params)[L"life"]));
		case Info::StateInfo::Type::INCSTAT :
			{
				std::wstring param = (*params)[L"value"];
				float amount = To<float>(param);
				IncStat::Category category = IncStat::Category::MAXHP;
				IncStat::ChargeType chargeType = IncStat::ChargeType::AMOUNT;

				param = (*params)[L"category"];
				if (param == L"MAXHP")
					category = IncStat::Category::MAXHP;
				else if (param == L"MAXMP")
					category = IncStat::Category::MAXMP;
				else if (param == L"ATKMAX")
					category = IncStat::Category::ATKMAX;
				else if (param == L"ATKMIN")
					category = IncStat::Category::ATKMIN;
				else if (param == L"DEF")
					category = IncStat::Category::DEF;
				else if (param == L"STR")
					category = IncStat::Category::STR;
				else if (param == L"INT")
					category = IncStat::Category::INT;
				else if (param == L"DEX")
					category = IncStat::Category::DEX;
				else if (param == L"VIT")
					category = IncStat::Category::VIT;
				else if (param == L"ATKSPD")
					category = IncStat::Category::ATKSPD;
				else {
					// Lunia_INFO(( L"[Stat::CreateState] category [%s] Not supported yet", param.c_str() ));
				}
				param = (*params)[L"type"];
				if (param == L"RATE")
					chargeType = IncStat::ChargeType::RATE;
				else if (param == L"AMOUNT")
					chargeType = IncStat::ChargeType::AMOUNT;
				else {
					Logger::GetInstance().Info(L"[StateGlobalFunction::CreateState] Wrong chargeType in INCSTAT.");
				}
				bool bFill = To<bool>( (*params)[L"bFill"].c_str() );

				return new IncStat(owner, category, chargeType, amount, bFill);
			}
		case Info::StateInfo::Type::INVISIBLE :
			return new Invisible(owner);
		case Info::StateInfo::Type::LESSMP :
			return new LessMP(owner, To<float>((*params)[L"probability"]) );
		case Info::StateInfo::Type::MAGICAMR :
			return new MagicAmr(owner, To<float>((*params)[L"regen"]), To<float>((*params)[L"defence"]));
		case Info::StateInfo::Type::MINIMIZE :
			return new Minimize(owner, To<float>((*params)[L"probability"]) );
		case Info::StateInfo::Type::MORPH :
			return new Morph( owner, StringUtil::Hash((*params)[L"objectName"].c_str()) );
		case Info::StateInfo::Type::MOVE :
			{
				std::wstring dir = (*params)[L"direction"];
				int direction = 4; //�Ʒ�
				if ( dir == L"F" ) direction = 0;
				else if ( dir == L"RF" ) direction = 1;
				else if ( dir == L"R" ) direction = 2;
				else if ( dir == L"RB" ) direction = 3;
				else if ( dir == L"B" ) direction = 4;
				else if ( dir == L"LB" ) direction = 5;
				else if ( dir == L"L" ) direction = 6;
				else if ( dir == L"LF" ) direction = 7;

				return new Move(owner, To<float>((*params)[L"speed"]), direction);
			}
		case Info::StateInfo::Type::PASSABLE :
			return new Passable(owner);
		case Info::StateInfo::Type::REFLECTION :
			return new Reflection(owner, To<float>((*params)[L"damage"]) );
		case Info::StateInfo::Type::REGENHP :
			{
				std::wstring param;
				RegenHp::ChargeType chargeType = RegenHp::ChargeType::AMOUNT;

				param = (*params)[L"type"];
				if (param == L"RATE")
					chargeType = RegenHp::ChargeType::RATE;
				else if (param == L"AMOUNT")
					chargeType = RegenHp::ChargeType::AMOUNT;
				else {
					Logger::GetInstance().Info(L"[StateGlobalFunction::CreateState] Wrong chargeType in REGENHP.");
					chargeType = RegenHp::ChargeType::AMOUNT;
				}

				float speed = To<float>((*params)[L"speed"]);

				bool isRegenWhenZero = false;
				StateParams::iterator iter = (*params).find(L"regenWhenZero");
				if( iter != (*params).end() ) {
					isRegenWhenZero = StringUtil::To<bool>(iter->second);
				}

				return new RegenHp(owner, chargeType, speed, isRegenWhenZero);
			}
		case Info::StateInfo::Type::REGENMP :
			{
				std::wstring param;
				RegenMp::ChargeType chargeType = RegenMp::ChargeType::AMOUNT;

				param = (*params)[L"type"];
				if (param == L"RATE")
					chargeType = RegenMp::ChargeType::RATE;
				else if (param == L"AMOUNT")
					chargeType = RegenMp::ChargeType::AMOUNT;
				else {
					Logger::GetInstance().Info(L"[StateGlobalFunction::CreateState] Wrong chargeType in REGENMP.");
					chargeType = RegenMp::ChargeType::AMOUNT;
				}

				float speed = To<float>((*params)[L"speed"]);

				bool isRegenWhenZero = false;
				StateParams::iterator iter = (*params).find(L"regenWhenZero");
				if( iter != (*params).end() ) {
					isRegenWhenZero = StringUtil::To<bool>(iter->second);
				}
				return new RegenMp(owner, chargeType, speed, isRegenWhenZero);
			}
		case Info::StateInfo::Type::RESIST :
			return new Resist(owner, (Constants::DamageType)(To<int>( (*params)[L"type"] )), To<float>( (*params)[L"amount"] ) );
		case Info::StateInfo::Type::RESURRECTION :
			return new Resurrection(owner, To<float>((*params)[L"regenRate"]) );
		case Info::StateInfo::Type::REVERSECMD :
			return new ReverseCmd(owner);
		case Info::StateInfo::Type::ROOT :
			return new Root(owner);
		case Info::StateInfo::Type::SELFHEAL :
			return new SelfHeal(owner, To<float>((*params)[L"hp"]) );
		case Info::StateInfo::Type::SLEEP :
			return new Lunia::XRated::Logic::Stat::Sleep(owner, To<float>((*params)[L"range"]), To<float>((*params)[L"duration"])
														, To<float>((*params)[L"curse"]), To<float>((*params)[L"buffRate"]) );
		case Info::StateInfo::Type::SLOW :
			return new Slow(owner, To<float>((*params)[L"speedFactor"]) );
		case Info::StateInfo::Type::SPREADHATE :
			return new SpreadHate(owner, To<float>((*params)[L"hate"]), To<float>((*params)[L"range"]));
		case Info::StateInfo::Type::STEP :
			{
				std::wstring dir = (*params)[L"direction"];
				int direction = 4; //�Ʒ�
				if ( dir == L"F" ) direction = 0;
				else if ( dir == L"RF" ) direction = 1;
				else if ( dir == L"R" ) direction = 2;
				else if ( dir == L"RB" ) direction = 3;
				else if ( dir == L"B" ) direction = 4;
				else if ( dir == L"LB" ) direction = 5;
				else if ( dir == L"L" ) direction = 6;
				else if ( dir == L"LF" ) direction = 7;

				return new Step(owner, To<float>((*params)[L"speed"]), direction);
			}
		case Info::StateInfo::Type::TAUNT :
			{
				float prob=1.0f;
				if ( (*params).find(L"probability") != (*params).end() )
					prob = To<float>((*params)[L"probability"]);

				return new Taunt(owner, prob, To<float>((*params)[L"range"]), To<float>((*params)[L"duration"]));
			}
		case Info::StateInfo::Type::WEAKEN :
			return new Weaken(owner, To<float>((*params)[L"lowerRate"]) );
		case Info::StateInfo::Type::IGNORESTATE:
			{
				StateParams::iterator iter = (*params).find(L"category");
				if( iter != (*params).end() ) {
					return new IgnoreState(owner, To<uint32>( (*params)[L"category"].c_str() ) );
				}
			}
			break;
		case Info::StateInfo::Type::IFHIT:
			{
				float probability = To<float>((*params)[L"probability"]);
				uint32 hash = StringUtil::Hash((*params)[L"stateBundleName"].c_str());
				bool bSelf = To<bool>((*params)[L"bSelf"].c_str());
				float coolDown = To<float>((*params)[L"coolDown"]);

				std::vector<IState*> newOptionalStates;

				for(std::vector<Database::Info::ActionInfo::State>::iterator itr = optionalStates->begin(); itr != optionalStates->end(); ++itr)
				{
					Stat::IState* state = Stat::CreateState((*itr).type, owner, &(*itr).params, &(*itr).optionalStates);
					if(state != NULL)
						newOptionalStates.push_back(state);
				}

				return new IfHit(owner, probability, bSelf, coolDown, hash, newOptionalStates);
			}
		case Info::StateInfo::Type::IFHITBY:
			{
				float probability = To<float>((*params)[L"probability"]);
				uint32 hash = StringUtil::Hash((*params)[L"stateBundleName"].c_str());
				bool bSelf = To<bool>((*params)[L"bSelf"].c_str());
				float coolDown = To<float>((*params)[L"coolDown"]);

				std::vector<IState*> newOptionalStates;

				for(std::vector<Database::Info::ActionInfo::State>::iterator itr = optionalStates->begin(); itr != optionalStates->end(); ++itr)
				{
					Stat::IState* state = Stat::CreateState((*itr).type, owner, &(*itr).params, &(*itr).optionalStates);
					if(state != NULL)
						newOptionalStates.push_back(state);
				}

				return new IfHitBy(owner, probability, bSelf, coolDown, hash, newOptionalStates);
			}
		case Info::StateInfo::Type::IFHEAL:
			{
				float probability = To<float>((*params)[L"probability"]);
				uint32 hash = StringUtil::Hash((*params)[L"stateBundleName"].c_str());
				bool bSelf = To<bool>((*params)[L"bSelf"].c_str());
				float coolDown = To<float>((*params)[L"coolDown"]);

				std::vector<IState*> newOptionalStates;

				for(std::vector<Database::Info::ActionInfo::State>::iterator itr = optionalStates->begin(); itr != optionalStates->end(); ++itr)
				{
					Stat::IState* state = Stat::CreateState((*itr).type, owner, &(*itr).params, &(*itr).optionalStates);
					if(state != NULL)
						newOptionalStates.push_back(state);
				}

				return new IfHeal(owner, probability, bSelf, coolDown, hash, newOptionalStates);
			}
		case Info::StateInfo::Type::IFCRIATK:
			{
				float probability = To<float>((*params)[L"probability"]);
				uint32 hash = StringUtil::Hash((*params)[L"stateBundleName"].c_str());
				bool bSelf = To<bool>((*params)[L"bSelf"].c_str());
				float coolDown = To<float>((*params)[L"coolDown"]);

				std::vector<IState*> newOptionalStates;

				for(std::vector<Database::Info::ActionInfo::State>::iterator itr = optionalStates->begin(); itr != optionalStates->end(); ++itr)
				{
					Stat::IState* state = Stat::CreateState((*itr).type, owner, &(*itr).params, &(*itr).optionalStates);
					if(state != NULL)
						newOptionalStates.push_back(state);
				}

				return new IfCriAtk(owner, probability, bSelf, coolDown, hash, newOptionalStates);
			}
		case Info::StateInfo::Type::IFCRIMAGIC:
			{
				float probability = To<float>((*params)[L"probability"]);
				uint32 hash = StringUtil::Hash((*params)[L"stateBundleName"].c_str());
				bool bSelf = To<bool>((*params)[L"bSelf"].c_str());
				float coolDown = To<float>((*params)[L"coolDown"]);

				std::vector<IState*> newOptionalStates;

				for(std::vector<Database::Info::ActionInfo::State>::iterator itr = optionalStates->begin(); itr != optionalStates->end(); ++itr)
				{
					Stat::IState* state = Stat::CreateState((*itr).type, owner, &(*itr).params, &(*itr).optionalStates);
					if(state != NULL)
						newOptionalStates.push_back(state);
				}

				return new IfCriMagic(owner, probability, bSelf, coolDown, hash, newOptionalStates);
			}
		case Info::StateInfo::Type::IFCASTSKILL:
			{
				float probability = To<float>((*params)[L"probability"]);
				uint32 hash = StringUtil::Hash((*params)[L"stateBundleName"].c_str());
				bool bSelf = To<bool>((*params)[L"bSelf"].c_str());
				float coolDown = To<float>((*params)[L"coolDown"]);
				std::wstring skillGroupName = (*params)[L"groupName"];

				std::vector<IState*> newOptionalStates;

				for(std::vector<Database::Info::ActionInfo::State>::iterator itr = optionalStates->begin(); itr != optionalStates->end(); ++itr)
				{
					Stat::IState* state = Stat::CreateState((*itr).type, owner, &(*itr).params, &(*itr).optionalStates);
					if(state != NULL)
						newOptionalStates.push_back(state);
				}

				return new IfCastSkill(owner, probability, bSelf, coolDown, hash, newOptionalStates, skillGroupName);
			}
			break;
		case Info::StateInfo::Type::RESETSKILLGROUPCOOLDOWN :
			{
				if(owner->GetType() == XRated::Constants::ObjectType::Player)
					return new ResetSkillGroupCooldown((Player*)owner, (*params).find(L"groupName")->second);
				break;
			}
		case Info::StateInfo::Type::DECREASESKILLCOOLDOWN:
			{
				if(owner->GetType() == XRated::Constants::ObjectType::Player)
					return new DecreaseSkillCooldown((Player*)owner, (*params).find(L"groupName")->second, StringUtil::ToFloat((*params).find(L"amount")->second.c_str()));
				break;
			}
		case Info::StateInfo::Type::RESETCOOLDOWN :
			{
				if(owner->GetType() != XRated::Constants::ObjectType::Player)
					break;

				Database::Info::StateInfo::RangeType rangeType = Database::Info::StateInfo::RangeType::Self;
				float range=0;
				if ( (*params).find(L"range") != (*params).end() )
					range = To<float>((*params)[L"range"]);

				if ( (*params).find(L"rangeType") != (*params).end() )
					rangeType = static_cast<Database::Info::StateInfo::RangeType>(To<int>((*params)[L"rangeType"]));

				uint8 targetCount=8;
				if ( (*params).find(L"targetCount") != (*params).end() )
					targetCount = To<uint8>((*params)[L"targetCount"]);
				return new ResetCooldown((Player*)owner, rangeType, range, targetCount);
			}
		//case Info::StateInfo::Type::CONVERTDMGTOHPMP:
		//	{
		//		int checkTarget = StringUtil::ToInt((*params).find(L"damagedTarget")->second.c_str());
		//		double rate = StringUtil::ToDouble((*params).find(L"rate")->second.c_str());
		//		std::wstring convertTo = (*params).find(L"convertTo")->second.c_str();
		//		int affectTarget = StringUtil::ToInt((*params).find(L"affectTarget")->second.c_str());

		//		return new ConvertDamageToHpMp(owner, convertTo == L"HP" ? ConvertDamageToHpMp::RecoveryType::Hp : ConvertDamageToHpMp::RecoveryType::Mp,
		//											checkTarget == 0 ? ConvertDamageToHpMp::TargetType::Owner : ConvertDamageToHpMp::TargetType::Other,
		//                                          checkTarget == 0 ? ConvertDamageToHpMp::TargetType::Owner : ConvertDamageToHpMp::TargetType::Other, rate);
		//	}
		case Database::Info::StateInfo::Type::ZOMBIE:
			{
				return	new Zombie(owner);
			}
		case Database::Info::StateInfo::Type::DMGTOHPMP:
			{
				float rate = 0.0f;
				float probability = 1.0f;

				Stat::DmgToHpMp::ConvertTo convertTo = Stat::DmgToHpMp::ConvertTo::Hp;

				std::map<std::wstring, std::wstring>::iterator itr;

				itr = (*params).find(L"rate");
				if(itr != (*params).end())
					rate = StringUtil::ToFloat(itr->second);

				itr = (*params).find(L"convertTo");
				if(itr != (*params).end())
				{
					if(itr->second == L"HP")
						convertTo = Stat::DmgToHpMp::ConvertTo::Hp;
					else
						convertTo = Stat::DmgToHpMp::ConvertTo::Mp;
				}

				itr = (*params).find(L"probability");
				if(itr != (*params).end())
					probability = StringUtil::ToFloat(itr->second);

				return	new Stat::DmgToHpMp(owner, convertTo, rate, probability);
			}
		case Database::Info::StateInfo::Type::ABSORBTOHPMP:
			{
				float rate = 0.0f;
				float probability = 1.0f;

				Stat::AbsorbToHpMp::ConvertTo convertTo = Stat::AbsorbToHpMp::ConvertTo::Hp;

				std::map<std::wstring, std::wstring>::iterator itr;

				itr = (*params).find(L"rate");
				if(itr != (*params).end())
					rate = StringUtil::ToFloat(itr->second);

				itr = (*params).find(L"convertTo");
				if(itr != (*params).end())
				{
					if(itr->second == L"HP")
						convertTo = Stat::AbsorbToHpMp::ConvertTo::Hp;
					else
						convertTo = Stat::AbsorbToHpMp::ConvertTo::Mp;
				}

				itr = (*params).find(L"probability");
				if(itr != (*params).end())
					probability = StringUtil::ToFloat(itr->second);

				return	new Stat::AbsorbToHpMp(owner, convertTo, rate, probability);
			}
		default :
			IState* returnValue = CreateState(type, (Object*)owner, params, optionalStates);
			if (returnValue) return returnValue;
		}
		Logger::GetInstance().Info("[Stat::CreateState<Actor>] Unknown state [{0}].", (int)type);
		return NULL;
	}

	/* optional state �� statebundle�� �߰� �Ϸ��� ����.
	template<> IState* CreateState(Info::StateInfo::Type type, Player* owner, StateParams* params,
										std::vector<Database::Info::ActionInfo::State>* optionalStates, Lunia::XRated::Serial creator)
	{
		return CreateStateImpl(type, owner, params, optionalStates, creator);
	}
	template<> IState* CreateState(Info::StateInfo::Type type, Player* owner, StateParams* params,
		std::vector<Database::Info::StateBundleInfo::State>* optionalStates, Lunia::XRated::Serial creator)
	{
		return CreateStateImpl(type, owner, params, optionalStates, creator);
	}
	template<class OptionalState> IState* CreateStateImpl(Info::StateInfo::Type type, Player* owner, StateParams* params,
										std::vector<OptionalState>* optionalStates, Lunia::XRated::Serial creator)
	*/
	template<> IState* CreateState(Info::StateInfo::Type type, Player* owner, StateParams* params,
										std::vector<Database::Info::ActionInfo::State>* optionalStates, Lunia::XRated::Serial creator)
	{
		switch (type) {
		case Info::StateInfo::Type::INCSKILLLEVEL :
			{
				uint32 hash = 0;
				int8 lv = 0;
				if ( (*params).find(L"skillHash") != (*params).end() )
					hash = To<uint32>((*params)[L"skillHash"]);
				if ( (*params).find(L"level") != (*params).end() )
					lv = (int8)(To<int>((*params)[L"level"]));
				return new IncSkillLevel(owner, hash, lv);
			}
		case Info::StateInfo::Type::RESETCOOLDOWN :
			{
				Database::Info::StateInfo::RangeType rangeType = Database::Info::StateInfo::RangeType::Self;
				float range=0;
				if ( (*params).find(L"range") != (*params).end() )
					range = To<float>((*params)[L"range"]);

				if ( (*params).find(L"rangeType") != (*params).end() )
					rangeType = static_cast<Database::Info::StateInfo::RangeType>(To<int>((*params)[L"rangeType"]));

				uint8 targetCount=8;
				if ( (*params).find(L"targetCount") != (*params).end() )
					targetCount = To<uint8>((*params)[L"targetCount"]);
				return new ResetCooldown(owner, rangeType, range, targetCount);
			}
		case Info::StateInfo::Type::RESETSKILLGROUPCOOLDOWN :
			return new ResetSkillGroupCooldown(owner, (*params).find(L"groupName")->second);
		case Info::StateInfo::Type::DECREASESKILLCOOLDOWN :
			return new DecreaseSkillCooldown(owner, (*params).find(L"groupName")->second, StringUtil::ToFloat((*params).find(L"amount")->second.c_str()));
		case Info::StateInfo::Type::REVIVE :
			std::cout << "StateGlobalFunction.cpp case Info::StateInfo::Type::REVIVE :" << std::endl;
			return new Revive(owner);
		case Info::StateInfo::Type::IGNORESTATE:
			{
				StateParams::iterator iter = (*params).find(L"category");
				if( iter != (*params).end() ) {
					return new IgnoreState(owner, To<uint32>( (*params)[L"category"].c_str() ) );
				}

			}
			break;
		case Info::StateInfo::Type::INVENTFAME:
			{
				StateParams::iterator iter = (*params).find(L"Text");
				if ( iter != (*params).end() ) 
				{
					return new InventFame(owner, iter->second.c_str() );
				}
			}
			break;
		case Info::StateInfo::Type::GOLDGAIN:
			{
				StateParams::iterator iter = (*params).find(L"Amount");
				int32 amount = 0;
				if ( iter != (*params).end() )
					amount = To<int32>(iter->second.c_str());
				if (amount < 0 ) amount = 0;
				return new GoldGain(owner, amount);
			}
			break;
		default :
			IState* returnValue = CreateState(type, (Actor*)owner, params, optionalStates);
			if (returnValue) return returnValue;
		}
		return NULL;
	}

	IState* CreateState(const IState* state, Object* owner,Lunia::XRated::Serial creator)
	{
		IState* newState = NULL;
		switch( state->GetType() ) {
		case Database::Info::StateInfo::Type::ADDBUFFWHENHIT :
			newState = new AddBuffWhenHit( *((AddBuffWhenHit*)state) );
			break;
		case Database::Info::StateInfo::Type::ADDBUFFWHENHITTED :
			newState = new AddBuffWhenHitted( *((AddBuffWhenHitted*)state) );
			break;
		case Database::Info::StateInfo::Type::ATTACK :
			switch ( ((Attack*)state)->GetAttackType() ) {
			case Attack::Type::Circle :
				newState = new AttackCircle( *((AttackCircle*)state) );
				break;
			case Attack::Type::Segment :
				newState = new AttackSegment( *((AttackSegment*)state) );
				break;
			case Attack::Type::Fanshape :
				newState = new AttackFanshape( *((AttackFanshape*)state) );
				break;
			case Attack::Type::Rectangle :
				newState = new AttackRectangle( *((AttackRectangle*)state) );
				break;
			}
			break;
		case Database::Info::StateInfo::Type::AREAHEAL :
			newState = new AreaHeal( *((AreaHeal*)state) );
			break;
		case Database::Info::StateInfo::Type::BLIND :
			newState = new Blind( *((Blind*)state));
			break;
		case Database::Info::StateInfo::Type::BLOCKUSINGSKILL :
			newState = new BlockUsingSkill(*((BlockUsingSkill*)state));
			break;
		case Database::Info::StateInfo::Type::CAMPFIRE :
			newState = new Campfire( *((Campfire*)state));
			break;
		case Database::Info::StateInfo::Type::CHANGEACTION :
			newState = new ChangeAction( *((ChangeAction*)state));
			break;
		case Database::Info::StateInfo::Type::CHARGE :
			newState = new Charge( *((Charge*)state));
			break;
		case Database::Info::StateInfo::Type::CLEAR :
			newState = new Clear( *((Clear*)state));
			break;
		case Database::Info::StateInfo::Type::CREATENPC :
			newState = new CreateNPC( *((CreateNPC*)state));
			break;
		case Database::Info::StateInfo::Type::CREATEOBJECT :
			newState = new CreateObject( *((CreateObject*)state));
			break;
		case Database::Info::StateInfo::Type::CREATEPROJECTILE :
			newState = new CreateProjectile( *((CreateProjectile*)state));
			break;
		case Database::Info::StateInfo::Type::CRIALLINC :
			newState = new CriAllInc( *((CriAllInc*)state));
			break;
		case Database::Info::StateInfo::Type::CRIATK :
			newState = new CriAtk( *((CriAtk*)state));
			break;
		case Database::Info::StateInfo::Type::CRIATKINC :
			newState = new CriAtkInc( *((CriAtkInc*)state));
			break;
		case Database::Info::StateInfo::Type::ELEMENTALATK :
			newState = new ElementalAtk( *((ElementalAtk*)state));
			break;
		case Database::Info::StateInfo::Type::CRIMAGIC :
			newState = new CriMagic( *((CriMagic*)state));
			break;
		case Database::Info::StateInfo::Type::CRIMAGICINC :
			newState = new CriMagicInc( *((CriMagicInc*)state));
			break;
		case Database::Info::StateInfo::Type::CRIRUSH :
			newState = new CriRush( *((CriRush*)state));
			break;
		case Database::Info::StateInfo::Type::DAMAGE :
			newState = new Damage( *((Damage*)state));
			break;
		case Database::Info::StateInfo::Type::DEFENCE :
			newState = new Defence( *((Defence*)state));
			break;
		case Database::Info::StateInfo::Type::DESTROY :
			newState = new Destroy( *((Destroy*)state));
			break;
		case Database::Info::StateInfo::Type::DESTROYENEMY :
			newState = new DestroyEnemy( *((DestroyEnemy*)state));
			break;
		case Database::Info::StateInfo::Type::DETECTINVISIBLE :
			newState = new DetectInvisible( *((DetectInvisible*)state));
			break;
		case Database::Info::StateInfo::Type::DEVINESHD :
			newState = new DevineShd( *((DevineShd*)state));
			break;
		case Database::Info::StateInfo::Type::DOT :
			newState = new Dot( *((Dot*)state));
			break;
		case Database::Info::StateInfo::Type::DOTMP :
			newState = new DotMP( *((DotMP*)state));
			break;
		case Database::Info::StateInfo::Type::CHOKE :
			newState = new Choke( *((Choke*)state));
			break;
		case Database::Info::StateInfo::Type::DOWN :
			newState = new Down( *((Down*)state));
			break;
		case Database::Info::StateInfo::Type::FLIED :
			newState = new Flied( *((Flied*)state));
			break;
		case Database::Info::StateInfo::Type::FREEZE :
			newState = new Freeze( *((Freeze*)state) );
			break;
		case Database::Info::StateInfo::Type::GHOST :
			newState = new Ghost( *((Ghost*)state));
			break;
		case Database::Info::StateInfo::Type::HATE :
			newState = new Hate( *((Hate*)state));
			break;
		case Database::Info::StateInfo::Type::INCDMG :
			newState = new IncDmg( *((IncDmg*)state));
			break;
		case Database::Info::StateInfo::Type::INCHEALAMOUNT :
			newState = new IncHealAmount( *((IncHealAmount*)state));
			break;
		case Database::Info::StateInfo::Type::IGNOREDMG :
			newState = new IgnoreDmg( *((IgnoreDmg*)state));
			break;
		case Database::Info::StateInfo::Type::IGNORESTUN :
			newState = new IgnoreStun( *((IgnoreStun*)state));
			break;
		case Database::Info::StateInfo::Type::IGNORECRITICALHIT:
			newState = new IgnoreCriticalHit( *((IgnoreCriticalHit*)state));
			break;
		case Database::Info::StateInfo::Type::IMMUNE :
			newState = new Immune( *((Immune*)state));
			break;
		case Database::Info::StateInfo::Type::INCLIFE :
			newState = new IncLife( *((IncLife*)state));
			break;
		case Database::Info::StateInfo::Type::INCSKILLLEVEL:
			newState = new IncSkillLevel( *((IncSkillLevel*)state));
			break;
		case Database::Info::StateInfo::Type::INCSTAT :
			newState = new IncStat( *((IncStat*)state));
			break;
		case Database::Info::StateInfo::Type::INVINCIBLE :
			newState = new Invincible( *((Invincible*)state) );
			break;
		case Database::Info::StateInfo::Type::INVISIBLE :
			newState = new Invisible( *((Invisible*)state) );
			break;
		case Database::Info::StateInfo::Type::KNOCKBACK :
			newState = new KnockBack( *((KnockBack*)state) );
			break;
		case Database::Info::StateInfo::Type::LESSMP :
			newState = new LessMP( *((LessMP*)state) );
			break;
		case Database::Info::StateInfo::Type::MAGICAMR:
			newState = new MagicAmr( *((MagicAmr*)state));
			break;
		case Database::Info::StateInfo::Type::MAKESTUN :
			newState = new MakeStun( *((MakeStun*)state));
			break;
		case Database::Info::StateInfo::Type::MINIMIZE :
			newState = new Minimize( *((Minimize*)state));
			break;
		case Database::Info::StateInfo::Type::MORPH :
			newState = new Morph( *((Morph*)state));
			break;
		case Database::Info::StateInfo::Type::MOVE :
			newState = new Move( *((Move*)state));
			break;
		case Database::Info::StateInfo::Type::PASSABLE :
			newState = new Passable( *((Passable*)state));
			break;
		case Database::Info::StateInfo::Type::REFLECTION :
			newState = new Reflection( *((Reflection*)state));
			break;
		case Database::Info::StateInfo::Type::REGENHP :
			newState = new RegenHp( *((RegenHp*)state));
			break;
		case Database::Info::StateInfo::Type::REGENMP :
			newState = new RegenMp( *((RegenMp*)state));
			break;
		case Database::Info::StateInfo::Type::RESETCOOLDOWN :
			newState = new ResetCooldown( *((ResetCooldown*)state));
			break;
		case Database::Info::StateInfo::Type::RESETSKILLGROUPCOOLDOWN :
			newState = new ResetSkillGroupCooldown( *((ResetSkillGroupCooldown*)state));
			break;
		case Database::Info::StateInfo::Type::DECREASESKILLCOOLDOWN :
			newState = new DecreaseSkillCooldown( *((DecreaseSkillCooldown*)state));
			break;
		case Database::Info::StateInfo::Type::RESIST :
			newState = new Resist( *((Resist*)state));
			break;
		case Database::Info::StateInfo::Type::RESURRECTION :
			newState = new Resurrection( *((Resurrection*)state));
			break;
		case Database::Info::StateInfo::Type::REVERSECMD :
			newState = new ReverseCmd( *((ReverseCmd*)state));
			break;
		case Database::Info::StateInfo::Type::REVIVE :
			newState = new Revive( *((Revive*)state));
			break;
		case Database::Info::StateInfo::Type::ROOT :
			newState = new Root( *((Root*)state));
			break;
		case Database::Info::StateInfo::Type::SELFHEAL :
			newState = new SelfHeal( *((SelfHeal*)state));
			break;
		case Database::Info::StateInfo::Type::SLEEP :
			newState = new Sleep( *((Sleep*)state));
			break;
		case Database::Info::StateInfo::Type::SLOW :
			newState = new Slow( *((Slow*)state));
			break;
		case Database::Info::StateInfo::Type::SPREADHATE :
			newState = new SpreadHate( *((SpreadHate*)state));
			break;
		case Database::Info::StateInfo::Type::STEP :
			newState = new Step( *((Step*)state));
			break;
		case Database::Info::StateInfo::Type::STUN :
			newState = new Stun( *((Stun*)state));
			break;
		case Database::Info::StateInfo::Type::TAUNT :
			newState = new Taunt( *((Taunt*)state));
			break;
		case Database::Info::StateInfo::Type::WEAKEN :
			newState = new Weaken( *((Weaken*)state));
			break;
		case Database::Info::StateInfo::Type::ZOMBIE :
			newState = new Zombie( *((Zombie*)state));
			break;
		case Info::StateInfo::Type::IGNORESTATE:			
			newState = new IgnoreState( *((IgnoreState*)state) );
			break;
		case Info::StateInfo::Type::IFHIT:
			newState = new IfHit( *((IfHit*)state) );
			break;
		case Info::StateInfo::Type::IFHITBY:
			newState = new IfHitBy( *((IfHitBy*)state) );
			break;
		case Info::StateInfo::Type::IFHEAL:
			newState = new IfHeal( *((IfHeal*)state) );
			break;
		case Info::StateInfo::Type::IFCRIATK:
			newState = new IfCriAtk( *((IfCriAtk*)state) );
			break;
		case Info::StateInfo::Type::IFCRIMAGIC:
			newState = new IfCriMagic( *((IfCriMagic*)state) );
			break;
		//case Info::StateInfo::Type::CONVERTDMGTOHPMP:
		//	newState = new ConvertDamageToHpMp( *((ConvertDamageToHpMp*)state) );
		//	break;
		case Database::Info::StateInfo::Type::DMGTOHPMP:
			newState = new DmgToHpMp(*((DmgToHpMp*)state));
			break;
		case Database::Info::StateInfo::Type::ABSORBTOHPMP:
			newState = new AbsorbToHpMp(*((AbsorbToHpMp*)state));
			break;
		case Database::Info::StateInfo::Type::INVENTFAME:
			newState = new InventFame(*((InventFame*)state));
			break;
		case Database::Info::StateInfo::Type::GOLDGAIN:
			newState = new GoldGain(*((GoldGain*)state));
			break;
		default :
			return NULL;
		}
		newState->SetOwner(owner);
		newState->SetCreator(creator);
		return newState;
	}

}	}	}	}
