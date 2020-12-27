#pragma once

#include "State.h"


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class CriAllInc 
		: public State<Actor> 
	{
	protected :
		float amount;

	public :
		CriAllInc(Actor* owner, float amount_)
			: State<Actor>(owner, Database::Info::StateInfo::Type::CRIALLINC, Database::Info::StateInfo::Sort::ATTACKSTATE)
			, amount( amount_ ) {}
		virtual ~CriAllInc() {}

		virtual void Initialize(IGameStateDatabase* db);
		virtual void Destroy();
	};

}	}	}	}
