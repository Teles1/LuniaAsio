#pragma once

#include "State.h"


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class Dot : public State<Actor> {
		float duration;
		float dps;
		float tick;
		float interval;
		Constants::DamageType dmgType;

	public :
		Dot(Actor* owner, float dur, float dmg, float inter, Constants::DamageType type)
			: State<Actor>(owner, Database::Info::StateInfo::Type::DOT, Database::Info::StateInfo::Sort::ORDINARYSTATE)
			, duration(dur), dps(dmg), dmgType(type), interval(inter), tick(0) {}
		virtual ~Dot() {}

		virtual void Initialize(IGameStateDatabase* db);
		virtual bool Do(float dt, IGameStateDatabase* db);
	};

}	}	}	}
