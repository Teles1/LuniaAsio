#pragma once

#include "State.h"


namespace AllM { namespace XRated { namespace Logic { namespace Stat {

	class BerserkStr : public State<Actor> {
	protected :
		float attack, defence, resist;

		bool bActivated;
	public :
		BerserkStr(Actor* owner, float atk, float def, float res);
		virtual ~BerserkStr() {}

		bool Do(float dt, IGameStateDatabase* db);

		void Activate();
		void DeActivate();
	};

}	}	}	}
