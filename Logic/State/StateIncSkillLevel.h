#pragma once

#include "State.h"


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class IncSkillLevel : public State<Player> {
		uint32 skillHash;
		int8 level;

	public :
		IncSkillLevel(Player* owner, uint32 hash, int8 lv)
			: State<Player>(owner, Database::Info::StateInfo::Type::INCSKILLLEVEL, Database::Info::StateInfo::Sort::DEFENCESTATE), skillHash(hash), level(lv) {}

		void Initialize(IGameStateDatabase* db);
		void Destroy();
	};

}	}	}	}
