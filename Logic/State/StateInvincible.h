#pragma once

#include "State.h"


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class Invincible : public State<Object> {
	protected :
		bool bDestroy;
		float duration;
	public :
		Invincible(Object* owner, float dur=0) : bDestroy(false), duration(dur), 
		   State<Object>(owner, Database::Info::StateInfo::Type::INVINCIBLE, Database::Info::StateInfo::Sort::ORDINARYSTATE) {}
		virtual ~Invincible() {}

	   virtual bool Do(float dt, IGameStateDatabase* db);
	   void Remove() { bDestroy = true; }
	};

}	}	}	}
