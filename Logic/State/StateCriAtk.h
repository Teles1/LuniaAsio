#pragma once

#include "State.h"


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class CriAtk : public State<Actor> {
	protected :
		float probability;

	public :
		CriAtk(Actor* owner, float prob)
			: State<Actor>(owner, Database::Info::StateInfo::Type::CRIATK, Database::Info::StateInfo::Sort::ATTACKSTATE)
			, probability(prob) {}
		virtual ~CriAtk() {}

		virtual void Initialize(IGameStateDatabase* db);
		virtual void Destroy();
	};

}	}	}	}
