#pragma once

#include "State.h"


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class IgnoreDmg : public State<Object> {
	protected :
		Lunia::Math::PseudoRandomFloat* randomFloat;
		float probability;

	public :
		IgnoreDmg(Object* owner, float p)
			: State<Object>(owner, Database::Info::StateInfo::Type::IGNOREDMG, Database::Info::StateInfo::Sort::DEFENCESTATE)
			, randomFloat( NULL )
			, probability(p) {}
		virtual ~IgnoreDmg() {}

		virtual void Initialize(IGameStateDatabase*);
		bool IsActivated();
	};

}	}	}	}
