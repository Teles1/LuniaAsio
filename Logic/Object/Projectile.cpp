#include "Projectile.h"
#include "Actor.h"
#include "../Action/Actions.h"
#include "../State/States.h"
#include "../GameState.h"
#include "../State/StateMakeStun.h"
#include <Info/Info/Database.h>
#include "../LogicDB/LogicDB.h"

using namespace Lunia::XRated::Logic::Action;
using namespace Lunia::XRated::Database;

namespace Lunia { namespace XRated {	namespace Logic {

	Projectile::Projectile(Serial serial, Info::ProjectileInfo* i, float minDmg, float maxDmg, const float3& pos, Serial creator, uint32 creatorActionHash, uint8 t, IGameStateDatabase& sd)
		:	Object(Lunia::XRated::Constants::ObjectType::Projectile)
		, creatorSerial(creator)
		, actionHash(creatorActionHash)
		, info(i), team(t), createdTime(0)
		, shootType( Info::ProjectileInfo::ShootingType::STRAIGHT )
		, dmgType ( DamageType::TARGET )
		, dmgMax(maxDmg), dmgMin(minDmg)
		, height( Map::StageMap::Height::WATER )
		, bAttackProcessed(false)
		, attackDelay(0)
	{
		hitObjects.reserve(10);
		stageData = &sd;
		objectData.GameObjectSerial = serial;
		objectData.Name = i->Name;
		objectData.NameHash = i->Hash;
		objectData.Radius = i->Radius;
		objectData.Position = pos;
	}

	Projectile::Projectile(Projectile& p)
		: Object(Lunia::XRated::Constants::ObjectType::Projectile)
	{
		stageData = p.stageData;
		objectData.GameObjectSerial = p.objectData.GameObjectSerial;
		creatorSerial = p.creatorSerial;
		actionHash = p.actionHash;
		objectData.Name = p.objectData.Name;
		objectData.NameHash = p.objectData.NameHash;
		objectData.Type = p.objectData.Type;
		objectData.Radius = p.objectData.Radius;
		objectData.Position = p.objectData.Position;
		objectData.Direction = p.objectData.Direction;
		team = p.team;
		shootType = p.shootType;
		dmgType = p.dmgType;
		createdTime = p.createdTime;
		height = p.height;
		bAttackProcessed = p.bAttackProcessed;
		attackDelay = p.attackDelay;
	}

	const float3& Projectile::GetDirection() const
	{
		return PhysicalFunctions::DirectionToDefinedDirection(objectData.Direction);
	}

	void Projectile::UpdateHitObject(float dt)
	{
		std::vector<HittedObject>::iterator i = hitObjects.begin(), end = hitObjects.end();
		for (; i != end ; ++i)
			i->duration += dt;
	}

	void Projectile::UpdateState(float dt, IGameStateDatabase* db)
	{
		//?????? ?? ???? ?????? ?? state?? ?????? ????????.
		BufferedStateList::iterator i = bufferedStates.begin();
		for( ; i != bufferedStates.end() ;) {
			if ( (*i).endTime >= 0 && (*i).endTime < createdTime + dt ) {
				//???? ?????? ??????
				RemoveState((*i).sort, (*i).id);
				i = bufferedStates.erase(i);
			} else ++i;
		}

		//State ????
		int cnt = (int)info->States.size();
		while ( cnt ) {
			--cnt;
			Info::ActionInfo::State& s = info->States[cnt];
			if ( s.startTime > createdTime && s.startTime <= createdTime + dt) {
				//?????????? ?????? ????????. ?????????? ????????.
				Logic::Stat::IState *state;
				state = LogicDBInstance().stateDB.CreateProjectileState(info->Hash, this, NULL, cnt, db );
				if ( state ) {
					AddState(state);
					if (s.startTime != s.endTime)	//?????? ?????? ????????.
						bufferedStates.push_back(BufferedState(state, state->GetSort(), s.endTime, state->GetId()));
				}
			}
		}
	}

	void Projectile::AddBundleToActor(const Database::Info::StateBundleInfo::Condition& condition, Actor* obj)
	{
		Database::Info::StateBundleInfo* info = Database::DatabaseInstance().InfoCollections.StateBundles.Retrieve(condition.hash);

		if (!info) {
			Logger::GetInstance().Info( L"[Projectile::AddBundleToActor] Unable to find statebundle ({0}) information.", condition.name.c_str());
			return;
		}

		std::vector<Actor*> l;
		switch (condition.rangeType) {
		case Database::Info::StateInfo::RangeType::Circle :
			stageData->GetAreaObject(objectData.Position, condition.params.at(0), info->allianceType, GetTeam(), this, l);
			break;
		//case Database::Info::StateInfo::RangeType::Rectangle :
			//stageData->GetAreaObject(objectData.Position, obj->GetDirection(), condition.params[0], condition.params[1], info->allianceType, GetTeam(), this, l);
			//return;
		//case Database::Info::StateInfo::RangeType::Segment :
			//stageData->GetAreaObject(objectData.Position, obj->GetDirection(), condition.params[0], info->allianceType, GetTeam(), this, l);
			//return;
		//case Database::Info::StateInfo::RangeType::FanShape :
		//	stageData->GetAreaObject(objectData.Position, obj->GetDirection(), condition.params[0], condition.params[1], condition.params[2], info->allianceType, GetTeam(), this, l);
		//	break;
		case Database::Info::StateInfo::RangeType::Self :
			if (obj)
				l.push_back(obj);
			break;
		default :
			return;
		}
		std::vector<Actor*>::iterator iActor = l.begin();
		uint16 lv = info->level;
		Actor* actor;
		for (; iActor != l.end() ; ++iActor) {
			actor = *iActor;
			if ( !actor->CheckState(Info::StateInfo::Type::GHOST) ) {
				if (actor->GetType() == Lunia::XRated::Constants::ObjectType::NonPlayer) { //NPC???? ???? NpcType?? ???????????? ????????.
					if ( !( (0x01 << ((NonPlayer*)actor)->GetNPCType()) & condition.targetNpcType) )
						continue;
				}
				//?????? ?????? ???? ????????.
				int type = XRated::Constants::AttackTargetType::Stand;
				if ( actor->CheckState(Database::Info::StateInfo::Type::FLIED) )
					type = XRated::Constants::AttackTargetType::Air;
				else if ( actor->CheckState(Database::Info::StateInfo::Type::DOWN) )
					type = XRated::Constants::AttackTargetType::Ground;

				if (type & condition.targetType)
				{
					actor->AddStateBundle(info,creatorSerial);
					//stageData->StateChanged(actor->GetSerial(), condition.hash, lv);
				}

			}
		}
	}

	void Projectile::ProcessStateBundle(float dt)
	{
		Database::Info::StateBundleInfo::ConditionList::iterator i = info->StateBundleConditions.begin();
		for ( ; i != info->StateBundleConditions.end() ; ++i) {
			Database::Info::StateBundleInfo::Condition& condition = (*i);
			if ( condition.startTime > createdTime-dt && condition.startTime <= createdTime )
				AddBundleToActor(condition);
		}
	}

	void Projectile::ProcessStateBundle(Actor* obj)
	{
		Database::Info::StateBundleInfo::ConditionList::iterator i = info->StateBundleConditions.begin();
		for ( ; i != info->StateBundleConditions.end() ; ++i) {
			Database::Info::StateBundleInfo::Condition& condition = (*i);
			if ( condition.startTime == -1 )
				AddBundleToActor(condition, obj);
		}
	}

	/**
		?????? : Hak
		Air Attack Combo ????..
	*/
	static uint32 DamageStateTypeUniqueSerial = 0;
	
	bool Projectile::Update(float dt, IGameStateDatabase* db)
	{
        UpdateState(dt, db);
		createdTime += dt;

		// ???? ????. ATTACK state ?????? ????????.
		if ( bAttackProcessed ) {
			attackDelay -= dt;
			if ( attackDelay <= 0 ) {
				attackDelay = 0;
				bAttackProcessed = false;
			}
		}

		bool createDamageState = false; 		//Hak. Air Attack
		bool returnValue = false;
		//Projectile ?? ?????? ????????
		if (createdTime >= info->Duration) {
			returnValue = true;
		} else {
			//???? ?????????? ???????????? ????????.
			std::vector<Actor*> l;
			db->GetAreaObject(objectData.Position, objectData.Radius, info->AllianceType, team, this, l);

			if (!info->DestroyWhenHit) // ?????????? ???? ?????? ????????.
				UpdateHitObject(dt);

			for (std::vector<Actor*>::iterator i = l.begin() ; i != l.end() ; ++i) {
				if ( (*i)->CheckState(Info::StateInfo::Type::DOWN) || (*i)->CheckState(Info::StateInfo::Type::GHOST) )	//?????? ?????????? ?????? ???? ??????. ???? ????
					continue;

				if ( (*i)->GetType() == Lunia::XRated::Constants::ObjectType::NonPlayer ) 
				{
					// ???????? ?????? ???? ??????.
					if ( ( (NonPlayer*)(*i) )->IsPetType() )
						continue;
				}

				//???????? ???? ???? ???????????? ????????.
				if (info->DestroyWhenHit) returnValue = true; //???????? ???????? ??????????????
				else {//???????? ???????? ???????? ????????.
					std::vector<HittedObject>::iterator hoi, hoend;
					Serial serial = (*i)->GetSerial();
					bool bContinue = false;
					hoi = hitObjects.begin(); hoend = hitObjects.end();
					for (; hoi != hoend ; ++hoi) {
						if (hoi->serial == serial) {//?????? ??????????.
							if (hoi->duration >= info->HitDelay) {//?? ???? ?????????
								//TODO : ?????? ???????????? ?????? ?? ???? ???? ?????? ????????.
								hoi->duration = ((hoi->duration / info->HitDelay) - static_cast<float>(static_cast<int>(hoi->duration / info->HitDelay))) * info->HitDelay;								
							}
							else {//???? ?????????? ???? ???? ???? ???? ?????? ??????.
								bContinue=true;
							}
							break;
						}
					}
					if (bContinue)
						continue;

					if (hoi == hoend) //?? ????????. ?????? ????.
						hitObjects.push_back( HittedObject(serial) );
				}

				std::vector<Info::ActionInfo::State>::iterator iter;
				int cnt = 0;
				for(iter = info->States.begin() ; iter != info->States.end() ; ++iter, ++cnt) {
					if ( (*iter).startTime == -1) {
						Stat::IState *state;
						state = LogicDBInstance().stateDB.CreateProjectileState(info->Hash, this, (Actor*)(*i), cnt, db, DamageStateTypeUniqueSerial );
						if ( state ) {
							switch ( (*iter).type ) {
							case Info::StateInfo::Type::ATTACK :
								if ( bAttackProcessed ) {
									delete state;
									continue;
								}
								// ???? ????. ATTACK state ?????? ????????.
								bAttackProcessed = true;
								attackDelay = info->HitDelay;
							case Info::StateInfo::Type::CREATEPROJECTILE :
							case Info::StateInfo::Type::AREAHEAL: // ???? ???? ?????? ???? by Yong-Eun Jung
								AddState(state, true);
								break;
							default :
								(*i)->AddState(state, true);
							}

							//Hak. Air Attack
							createDamageState = true;
						}
					}
				}
				ProcessStateBundle((*i));
			}

			//StateBundle?? ???? ??????.
			ProcessStateBundle(dt);

			//???? ??????.
			switch ( info->ShootType ) {
			case Info::ProjectileInfo::ShootingType::FOLLOWTARGET :
			case Info::ProjectileInfo::ShootingType::HEAL :
			case Info::ProjectileInfo::ShootingType::HIGHANGLE :
			case Info::ProjectileInfo::ShootingType::STRAIGHT :
			case Info::ProjectileInfo::ShootingType::FLOATING :
				{
					float3 pos = objectData.Position + (objectData.Direction * dt * info->Speed);
					//???? ?????????? ???????????? ????????.
					Map::StageMap* stageMap = db->GetStageMap();
					if (stageMap->IsAbletoLocate(pos, objectData.Radius, height) ) {
						SetPosition( pos );
						//objectData.Position = pos;
						//db->ObjectMoved(this);
					} else returnValue = true; //????????
				}
				break;
			}

			//Hak. Air Attack
			if (createDamageState)
				++DamageStateTypeUniqueSerial;
		}

		Object::Update(dt, db);
		return returnValue;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//ProjectileFollow class implementation
	ProjectileFollow::ProjectileFollow(Serial serial, Database::Info::ProjectileInfo* i, float minDmg, float maxDmg
			, const float3& pos, Serial creator, uint32 creatorActionHash, uint8 team, IGameStateDatabase& sd, Serial target)
			: Projectile(serial, i, minDmg, maxDmg, pos, creator, creatorActionHash, team, sd)
			, targetSerial(target), lastUpdatedTick(0)
	{
		shootType = Info::ProjectileInfo::ShootingType::FOLLOWTARGET;
		height = Map::StageMap::Height::WATER;
	}

	bool ProjectileFollow::Update(float dt, IGameStateDatabase* db)
	{
		lastUpdatedTick += dt;

		if (lastUpdatedTick >= 0.1f) {
			lastUpdatedTick -= 0.1f;
			if (targetSerial) {//?????? ???? ??????
				Object* target = db->Retrieve(targetSerial);
				if (target)
					objectData.Direction = PhysicalFunctions::GetSpinnedDirection(target->GetPosition() - objectData.Position, objectData.Direction, info->SpinLimit);
				else targetSerial = 0;
			}
		}
		return Projectile::Update(dt, db);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//ProjectileHeal class implementation
	ProjectileHeal::ProjectileHeal(Serial serial, Database::Info::ProjectileInfo* i, float minDmg, float maxDmg
			, const float3& pos, Serial creator, uint32 creatorActionHash, uint8 team, IGameStateDatabase& sd, Serial target)
			: Projectile(serial, i, minDmg, maxDmg, pos, creator, creatorActionHash, team, sd)
			, targetSerial(target), lastUpdatedTick(0)
	{
		shootType = Info::ProjectileInfo::ShootingType::HEAL;
		height = Map::StageMap::Height::WATER;
	}

	bool ProjectileHeal::Update(float dt, IGameStateDatabase* db)
	{
		lastUpdatedTick += dt;

		if (lastUpdatedTick >= 0.1f) {
			lastUpdatedTick -= 0.1f;
			if (targetSerial) {//?????? ???? ??????
				Object* target = db->Retrieve(targetSerial);
				if (target)
					objectData.Direction = PhysicalFunctions::GetSpinnedDirection(target->GetPosition() - objectData.Position, objectData.Direction, info->SpinLimit);
				else targetSerial = 0;
			}
		}
		return Projectile::Update(dt, db);
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//ProjectileHighAngle class implementation
	ProjectileHighAngle::ProjectileHighAngle(Serial serial, Database::Info::ProjectileInfo* i, float dur, float minDmg, float maxDmg
				, const float3& pos, Serial creator, uint32 creatorActionHash, uint8 team, IGameStateDatabase& sd)
				: Logic::Projectile(serial, i, minDmg, maxDmg, pos, creator, creatorActionHash, team, sd), duration ( dur )
	{
		shootType = Info::ProjectileInfo::ShootingType::HIGHANGLE;
		height = Map::StageMap::Height::FENCE;
	}

	bool ProjectileHighAngle::Update(float dt, IGameStateDatabase* db)
	{
		bool returnValue = false;
		createdTime += dt;

		//Projectile ?? ???????? ?? ?????? ?? ??????????
		if (createdTime >= duration) {
			returnValue = true;
			std::vector<Info::ActionInfo::State>::iterator iter;
			for(iter = info->States.begin() ; iter != info->States.end() ; ++iter) {
				if ( (*iter).startTime == -1) {
#ifdef _SINGLE
					if ( (*iter).type == Info::StateInfo::Type::DAMAGE || (*iter).type == Info::StateInfo::Type::ROOT ) {
						Lunia_INFO(( L"[ProjectileHighAngle::Update] Fall type cannot have DAMAGE/ROOT state." ));
						continue;
					}
#endif
					Stat::IState *state;
					state = Stat::CreateState((*iter).type, (Projectile*)this, NULL, &(*iter).params);
					AddState(state, true);
				}
			}

		}
		//???? ?????????? ???????????? ????????.
		Map::StageMap* stageMap = db->GetStageMap();
		float3 pos = objectData.Position + (objectData.Direction * dt * info->Speed);
		if (stageMap->IsAbletoLocate(pos, objectData.Radius, height) ) {
			SetPosition( pos );
			//db->ObjectMoved(this);
		} else
			returnValue = true;	// true?? ???????? ?????? ?????? ????????.

		Object::Update(dt, db);
		return returnValue;
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//ProjectileFall class implementation
	ProjectileFall::ProjectileFall(Serial serial, Database::Info::ProjectileInfo* i, float minDmg
				, float maxDmg, const float3& pos, Serial creator, uint32 creatorActionHash, uint8 team, IGameStateDatabase& sd)
				: Logic::Projectile(serial, i, minDmg, maxDmg, pos, creator, creatorActionHash, team, sd)
	{
		shootType = Info::ProjectileInfo::ShootingType::FALL;
		height = Map::StageMap::Height::UNPASSABLE;
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//ProjectileFloating class implementation
	ProjectileFloating::ProjectileFloating(Serial serial, Database::Info::ProjectileInfo* i, float minDmg, float maxDmg
			, const float3& pos, Serial creator, uint32 creatorActionHash, uint8 team, float delay, IGameStateDatabase& sd)
			: Logic::Projectile(serial, i, minDmg, maxDmg, pos, creator, creatorActionHash, team, sd)
			, changeDirectionDelay(delay), lastChangeTime(0)
	{
		shootType = Info::ProjectileInfo::ShootingType::FLOATING;
	}

	bool ProjectileFloating::Update(float dt, IGameStateDatabase* db)
	{
		lastChangeTime += dt;
		if (lastChangeTime >= changeDirectionDelay) {
			lastChangeTime -= changeDirectionDelay;
			//?????? ?????? ?????? ????.
			objectData.Direction = PhysicalFunctions::RotatePosition(objectData.Direction, db->GetRandomInt(0, 360));
		}

		return Projectile::Update(dt, db);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//ProjectileCircular class implementation
	ProjectileCircular::ProjectileCircular(
		Serial serial, Lunia::XRated::Serial target, 
		Database::Info::ProjectileInfo* i, 
		float minDmg, 
		float maxDmg, 
		const float3& pos, 
		const float3& createdPosition, 
		const float3& centerPosition, 
		Serial creator, 
		uint32 creatorActionHash,
		uint8 team, 
		IGameStateDatabase& sd)
			: Logic::Projectile(serial, i, minDmg, maxDmg, pos, creator, creatorActionHash, team, sd)
			, targetSerial(target)
			, createdPos(createdPosition)
			, centerPos(centerPosition)
	{
		shootType = Info::ProjectileInfo::ShootingType::CIRCULAR;
	}

	bool ProjectileCircular::Update(float dt, IGameStateDatabase* db)
	{
		if ( targetSerial ) {
			Actor* targetObject = db->RetrieveActor(targetSerial);
			if ( targetObject) {
				centerPos = targetObject->GetPosition();
				SetPosition(Lunia::XRated::PhysicalFunctions::RotatePosition(createdPos, (int)((float)info->SpinLimit * createdTime)) + centerPos );
			} else { //?????? ?????? ?? ?????????? ?????? ??????.
				targetSerial = 0;
			}
		} else {
			SetPosition(Lunia::XRated::PhysicalFunctions::RotatePosition(createdPos, (int)((float)info->SpinLimit * createdTime)) + centerPos );
		}
		return Projectile::Update(dt, db);
	}


}	}	}
