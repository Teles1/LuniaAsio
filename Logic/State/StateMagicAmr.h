#pragma once

#include "State.h"


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class MagicAmr : public State<Actor> {
	protected :
		float regenRate, defence;
		float currentTime;

	public :
		MagicAmr(Actor* owner, float regen, float def)
			: State<Actor>(owner, Database::Info::StateInfo::Type::MAGICAMR, Database::Info::StateInfo::Sort::ORDINARYSTATE)
			, regenRate(regen), defence(def), currentTime(0) {}
		virtual ~MagicAmr() {}

		virtual void Initialize(IGameStateDatabase* db);
		virtual bool Do(float dt, IGameStateDatabase* db);
		virtual void Destroy();
	};


}	}	}	}
