#pragma once

#include "State.h"


namespace AllM { namespace XRated { namespace Logic { namespace Stat {

	class RecoverStun : public State<Object> {
	protected :
		AllM::Math::PseudoRandomFloat* randomFloat;
		float probability;

	public :
		RecoverStun(Object* owner, float prob)
			: State<Object>(owner, Database::Info::StateInfo::Type::RECOVERSTUN, Database::Info::StateInfo::Sort::DEFENCESTATE)
			, randomFloat( NULL ), probability(prob) {}
		virtual ~RecoverStun() {}

		virtual void Initialize(IGameStateDatabase*);
		bool IsActivated();
	};

}	}	}	}
