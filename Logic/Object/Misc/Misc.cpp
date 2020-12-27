#include "Misc.h"
#include <Info/Info/Database.h>
#include "../../GameState.h"
#include "../../State/States.h"
#include "../Player.h"
using namespace Lunia::XRated::Database::Info;

namespace Lunia { namespace XRated {	namespace Logic {

	Misc::Misc() : Actor(Lunia::XRated::Constants::ObjectType::Misc)
	{
		objectData.Name = L"Misc";
	}

	bool Misc::DoDamage(Object* who, uint32, float, float, float, Constants::DamageType, uint32 /*stateID*/, Constants::AttackStateType /*attackType*/, unsigned int& sFlag)
	{
		--status.Hp;

		if ( !who ) {
			stageData->StatusChanged(objectData.GameObjectSerial, objectData.Position, objectData.Direction, status.Hp,
									status.Mp, 0, 0);
		} else if ( who->GetType() == Lunia::XRated::Constants::ObjectType::Projectile ) {
			stageData->StatusChanged(objectData.GameObjectSerial, objectData.Position, objectData.Direction, status.Hp,
									status.Mp, ((Projectile*)who)->GetCreatorSerial(), who->GetNameHash(), sFlag);
		} else if (who->GetType() == Lunia::XRated::Constants::ObjectType::Player ||
			who->GetType() == Lunia::XRated::Constants::ObjectType::NonPlayer ||
			who->GetType() == Lunia::XRated::Constants::ObjectType::Vehicle ||
			who->GetType() == Lunia::XRated::Constants::ObjectType::Structure)
		{
			stageData->StatusChanged(objectData.GameObjectSerial, objectData.Position, objectData.Direction, status.Hp,
									status.Mp, who->GetSerial(), ((Actor*)who)->GetActionNameHash(), sFlag);
		} else {

		}

		if (status.Hp == 0) {
			float3 posRevision[4];
			posRevision[0].SetVector(0, 0, -15);
			posRevision[1].SetVector(0, 0, 15);
			posRevision[2].SetVector(15, 0, 0);
			posRevision[3].SetVector(-15, 0, 0);

			float r;
			float prob=0;
			if(info->BonusBox) {
				std::vector<NonPlayerInfo::Item>& items = info->Items;
				std::vector<NonPlayerInfo::Item>::iterator item, itemEnd = items.end();

				const PlayerMap &players = stageData->GetPlayers();

				int posCnt = 0;
				for ( PlayerMap::const_iterator i = players.begin(); i != players.end() ; ++i, ++posCnt ) {
					if((*i).second->IsGainNormal() == false)
						continue;

					if (posCnt > 3) posCnt = 0;
					r = stageData->GetRandomFloat();
					prob = 0;
					for (item = items.begin(); item != itemEnd ; ++item) {
						if (item->Probability==0.0f) continue;
						prob += (*item).Probability;
						if ( r <= prob ) {
							float3 pos = objectData.Position;
							pos.y = 0;
							float3 direction = who->GetDirection();
							stageData->CreateItem((*item).Hash, pos + posRevision[posCnt], direction, (*i).second->GetSerial(), 1, 0, false);
							break;
						}
					}
				}
			} else {
				std::vector<NonPlayerInfo::Item>& items = info->Items;
				std::vector<NonPlayerInfo::Item>::iterator item = items.begin(), itemEnd = items.end();

				r = stageData->GetRandomFloat();

				for (; item != itemEnd ; ++item) {
					if (item->Probability==0.0f) continue;
					prob += (*item).Probability;
					if ( r <= prob ) {
						float3 pos = objectData.Position;
						pos.y = 0;
						float3 direction = who->GetDirection();
						stageData->CreateItem((*item).Hash, pos, direction, uint16(1));
						break;
					}
				}
			}
		}

		return true;
	}

	bool Misc::LoadStatus(uint32 misc)
	{
		info = Database::DatabaseInstance().InfoCollections.Miscs.Retrieve(misc);
		if ( !info ) {
			Logger::GetInstance().Error( L"[Misc::LoadStatus] unknown misc. [{0}]", misc );
			return false;
		}

		objectData.Name = info->Name;
		objectData.NameHash = info->NameHash;
		objectData.Radius = info->Radius;
		status.Hp = status.MaxHp = (float)info->Hp;

		Stat::IState *state;
		for (std::vector<ActionInfo::State>::iterator i = info->States.begin() ; i != info->States.end() ; ++i) {
			state = Stat::CreateState<Actor>((*i).type, (Actor*)this, &(*i).params, &(*i).optionalStates);
			AddState(state);
		}
		return true;
	}

	bool Misc::Update(float dt, IGameStateDatabase* db)
	{
		Object::Update(dt, db);
		if ( duration > 0 ) {
            duration -= dt;
			if (duration <= 0)
				return true;
		}

		if (status.Hp <= 0) {
			//std::vector<NonPlayerInfo::Item>& items = info->Items;
			//std::vector<NonPlayerInfo::Item>::iterator item = items.begin(), itemEnd = items.end();

			//float r = db->GetRandomFloat();
			//float prob=0;
			//for (; item != itemEnd ; ++item) {
			//	prob += (*item).Probability;
			//	if ( r <= prob ) {
			//		float3 pos = objectData.Position;
			//		pos.y = 0;
			//		db->CreateItem((*item).Hash, pos, objectData.Direction);
			//		break;
			//	}
			//}
			return true;
		}
		return false;
	}

	bool Misc::SetAction(uint32, Action::Action::IActionParam*, bool)
	{
		return false;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// MiscFinite
	//bool MiscFinite::Update(float dt, IGameStateDatabase* db)
	//{
	//	return Misc::Update(dt, db);
	//}


	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Misc Invincible
	//MiscTemp::MiscTemp()
	//{
	//	stateFlags[Database::Info::StateInfo::Type::INVINCIBLE] = 1;
	//}

}	}	}
