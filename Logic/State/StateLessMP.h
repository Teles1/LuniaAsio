#pragma once

#include "State.h"


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class LessMP : public State<Actor> {
	protected :
		float probability;

	public :
		LessMP(Actor* owner, float prob)
			:	State<Actor>(owner, Database::Info::StateInfo::Type::LESSMP, Database::Info::StateInfo::Sort::DEFENCESTATE),
				probability(prob)
		{
		}
		
		virtual ~LessMP()
		{
		}

		virtual void Initialize(IGameStateDatabase* db);
		virtual void Destroy();
	};

}	}	}	}
