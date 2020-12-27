#pragma once

#include "State.h"


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class MakeStun : public State<Object> {
		bool bArea;
		float range;
		float stunLength;

	public :
		MakeStun(Object* owner, bool area, float r, float stun) : bArea(area), range(r), stunLength(stun),
		   State<Object>(owner, Database::Info::StateInfo::Type::MAKESTUN, Database::Info::StateInfo::Sort::ORDINARYSTATE) {}
		virtual ~MakeStun() {}

		virtual bool Do(float dt, IGameStateDatabase* db);
	};

}	}	}	}
