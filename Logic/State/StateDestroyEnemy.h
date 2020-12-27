#pragma once

#include "State.h"


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class Destroy : public State<Actor> {
	public :
		Destroy(Actor* owner) :
		   State<Actor>(owner, Database::Info::StateInfo::Type::DESTROY, Database::Info::StateInfo::Sort::ORDINARYSTATE) {}

		   void Initialize(IGameStateDatabase*) { owner->SetHP(0); }
	};

	class DestroyEnemy : public State<Object> {
	public :
		enum AreaType {
			ONE, RANGE, SCREEN, ALLINGAME
		};

	private :
		Constants::ObjectType targetType;
		AreaType areaType;
		float radius;	//대상 범위가 원일때만

		void DestroyProjectile(IGameStateDatabase* db);
		void DestroyNPC(IGameStateDatabase* db);

	public :
		DestroyEnemy(Object* owner, Constants::ObjectType target, AreaType at, float r)
			: State<Object>(owner, Database::Info::StateInfo::Type::DESTROYENEMY, Database::Info::StateInfo::Sort::ORDINARYSTATE)
			, targetType(target), areaType(at), radius(r) {}

		bool Do(float dt, IGameStateDatabase* db);
	};

}	}	}	}
