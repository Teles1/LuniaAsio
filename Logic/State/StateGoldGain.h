#pragma once

#include "State.h"


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class GoldGain : public State<Player> {
	protected :
		uint32 money;
	public :
		GoldGain(Player* owner, uint32 amount) : money(amount),
		   State<Player>(owner, Database::Info::StateInfo::Type::GOLDGAIN, Database::Info::StateInfo::Sort::ORDINARYSTATE) {}

		void Initialize(IGameStateDatabase* db);
		bool Do(float dt, IGameStateDatabase* db) { return true; }
	};

}	}	}	}
