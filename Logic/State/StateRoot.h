#pragma once

#include "State.h"


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class Root : public State<Actor> {
	public :
		Root(Actor* owner)
			: State<Actor>(owner, Database::Info::StateInfo::Type::ROOT, Database::Info::StateInfo::Sort::ORDINARYSTATE) {}
		virtual ~Root() {}

		virtual bool Do(float dt, IGameStateDatabase* db);
	};

}	}	}	}
