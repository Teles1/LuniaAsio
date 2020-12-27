#pragma once

#include "State.h"


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class KnockBack : public State<Actor> {
	protected :
		float3 direction;
		float distance;

	public :
		KnockBack(Actor* owner, float3 dir, float dis) :
			State<Actor>(owner, Database::Info::StateInfo::Type::KNOCKBACK, Database::Info::StateInfo::Sort::ORDINARYSTATE),
			direction(dir), distance(dis) {}
		virtual ~KnockBack() {}

		virtual void Initialize(IGameStateDatabase* db);
		virtual bool Do(float dt, IGameStateDatabase* db);
	};

}	}	}	}
