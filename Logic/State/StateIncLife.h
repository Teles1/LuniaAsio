#pragma once

#include "State.h"


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class IncLife : public State<Actor> {
		uint16 life;

	public :
		IncLife(Actor* owner, uint16 l)
			: State<Actor>(owner, Database::Info::StateInfo::Type::INCLIFE, Database::Info::StateInfo::Sort::ORDINARYSTATE), life(l) {}
		virtual ~IncLife() {}

		virtual bool Do(float dt, IGameStateDatabase* db);
	};

}	}	}	}
