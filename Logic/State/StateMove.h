#pragma once

#include "State.h"


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class Move : public State<Actor> {
		int direction;
		float speed;
		float3 directionVector;

	public :
		Move(Actor* owner, float s, int dir)
			: State<Actor>(owner, Database::Info::StateInfo::Type::MOVE, Database::Info::StateInfo::Sort::ORDINARYSTATE)
			, speed(s), direction(dir) {}
		virtual ~Move() {}

		void Initialize(IGameStateDatabase* db);
		bool Do(float dt, IGameStateDatabase* db);
	};

}	}	}	}
