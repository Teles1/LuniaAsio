#pragma once

#include "State.h"


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class Step : public State<Actor> {
		int direction;
		float speed;
		float3 directionVector;
		std::wstring strDirection;

	public :
		Step::Step(Actor* owner, float s, int dir)
			: State<Actor>(owner, Database::Info::StateInfo::Type::STEP, Database::Info::StateInfo::Sort::ORDINARYSTATE)
			, speed (s), direction(dir) {}
		virtual ~Step() {}

		void Initialize(IGameStateDatabase*);
		bool Do(float dt, IGameStateDatabase* db);
	};

}	}	}	}
