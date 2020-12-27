#pragma once

#include "State.h"


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class DetectInvisible : public State<Object> {
		float range;

	public :
		DetectInvisible(Object* owner, float r) :
		   State<Object>(owner, Database::Info::StateInfo::Type::DETECTINVISIBLE, Database::Info::StateInfo::Sort::DEFENCESTATE), range(r) {}

		//virtual void Initialize(IGameStateDatabase* db);
		//virtual bool Do(float dt, IGameStateDatabase* db);
		//virtual void Destroy();
	};

}	}	}	}
