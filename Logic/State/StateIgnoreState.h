#pragma once

#include "State.h"


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class IgnoreState : public State<Actor>
	{
		const uint32 ignoreCategory;
	public:
		IgnoreState(Actor* owner,uint32 category)
			: State<Actor>(owner, Database::Info::StateInfo::Type::IGNORESTATE, Database::Info::StateInfo::Sort::DEFENCESTATE)
			 ,ignoreCategory(category) {}
		virtual ~IgnoreState() {}

		void Initialize(IGameStateDatabase* db);
		void Destroy();
	};

}	}	}	}
