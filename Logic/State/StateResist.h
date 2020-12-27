#pragma once

#include "State.h"


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class Resist : public State<Actor> {
		Constants::DamageType dmgType;
		float rate;
	public :
		Resist(Actor* owner, Constants::DamageType type, float r)
			: State<Actor>(owner, Database::Info::StateInfo::Type::RESIST, Database::Info::StateInfo::Sort::DEFENCESTATE)
			, dmgType(type), rate(r) {}
		virtual ~Resist() {}

		virtual void Initialize(IGameStateDatabase* db);
		virtual void Destroy();
	};

}	}	}	}
