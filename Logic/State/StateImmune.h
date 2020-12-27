#pragma once

#include "State.h"


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class Immune : public State<Actor> {
		uint16 immune;

	public :
		Immune(Actor* owner, uint16 i)
			: State<Actor>(owner, Database::Info::StateInfo::Type::IMMUNE, Database::Info::StateInfo::Sort::DEFENCESTATE)
			, immune(i) {}
		virtual ~Immune() {}

		virtual void Initialize(IGameStateDatabase* db);
		virtual void Destroy();
	};

}	}	}	}
