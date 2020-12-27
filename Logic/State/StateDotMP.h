#pragma once

#include "State.h"


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class DotMP : public State<Actor> {
		float duration;
		float dps;
		float tick;
		float interval;

	public :
		DotMP(Actor* owner, float dur, float dmg, float inter)
			: State<Actor>(owner, Database::Info::StateInfo::Type::DOTMP, Database::Info::StateInfo::Sort::ORDINARYSTATE)
			, duration(dur), dps(dmg), interval(inter), tick(0) {}
		virtual ~DotMP() {}

		virtual void Initialize(IGameStateDatabase* db);
		virtual bool Do(float dt, IGameStateDatabase* db);
	};

}	}	}	}
