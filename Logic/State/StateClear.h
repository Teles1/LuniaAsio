#pragma once

#include "State.h"


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class Clear : public State<Actor> {
		float range;
		float buffRate;
		int sort;
		uint32 subCategoryHash;

	public :
		Clear(Actor* owner, float r, float rate, int s, uint32 sub)
			: State<Actor>(owner, Database::Info::StateInfo::Type::CLEAR, Database::Info::StateInfo::Sort::ORDINARYSTATE)
			, range(r), buffRate(rate), sort(s), subCategoryHash(sub) {}
		virtual ~Clear() {}

		bool Do(float dt, IGameStateDatabase* db);
	};

}	}	}	}
