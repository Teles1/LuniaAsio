#pragma once

#include "State.h"


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class Campfire : public State<Actor> {
	public :
		Campfire(Actor* owner) :
		   State<Actor>(owner, Database::Info::StateInfo::Type::CAMPFIRE, Database::Info::StateInfo::Sort::DEFENCESTATE) {}
		virtual ~Campfire() {}
	};

}	}	}	}
