#pragma once

#include "State.h"


namespace AllM { namespace XRated { namespace Logic { namespace Stat {

	/** Charge mp by hp*/
	class HolyMP : public State<Actor> {
		float mpAmount;

	public :
		HolyMP(Actor* owner, float mp)
			: State<Actor>(owner, Database::Info::StateInfo::Type::HOLYMP, Database::Info::StateInfo::Sort::ORDINARYSTATE)
			, mpAmount(mp) {}
		virtual ~HolyMP() {}

		virtual bool Do(float dt, IGameStateDatabase* db);
	};

}	}	}	}
