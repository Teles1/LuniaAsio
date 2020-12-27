#pragma once

#include "State.h"


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class CreateNPC : public State<Actor> {
	protected :
		uint32 name;
		float2 startPos;
		int idleType, transType;
		uint16 defenceLocation, path, team, alliance;

	public :
		CreateNPC(Actor* owner, uint32 n, float2 pos, int it, int tt, uint16 dl, uint16 p, uint16 a, uint16 initTeam ) :
		   State<Actor>(owner, Database::Info::StateInfo::Type::CREATENPC, Database::Info::StateInfo::Sort::ORDINARYSTATE),
		   name(n), startPos(pos), idleType(it), transType(tt), defenceLocation(dl), path(p), alliance(a), team( initTeam ) {}
		virtual ~CreateNPC() {}

		bool Do(float dt, IGameStateDatabase* db);
	};

}	}	}	}
