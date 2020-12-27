#pragma once

#include "State.h"


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class Revive : public State<Player> {
	public :
		Revive(Player* owner) :
		   State<Player>(owner, Database::Info::StateInfo::Type::REVIVE, Database::Info::StateInfo::Sort::DEFENCESTATE) {}

		   bool Do(float /*dt*/, IGameStateDatabase* /*db*/) { return true; }
	};

}	}	}	}
