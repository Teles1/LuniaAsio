#pragma once

#include "State.h"


namespace AllM { namespace XRated { namespace Logic { namespace Stat {

	class NearHeal : public State<Actor> {
		float range;
		float damage;

	public :
		NearHeal(Actor* owner, float r, float dmg)
			: State<Actor>(owner, Database::Info::StateInfo::Type::NEARHEAL, Database::Info::StateInfo::Sort::ORDINARYSTATE)
			, range(r), damage(dmg) {}
		virtual ~NearHeal() {}

		virtual bool Do(float dt, IGameStateDatabase* db);
	};

}	}	}	}
