#pragma once

#include "State.h"


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class Base : public State<Object> {
	public :
		Base(Object* owner) :
		   State<Object>(owner, Database::Info::StateInfo::Type::BASE, Database::Info::StateInfo::Sort::ORDINARYSTATE) {}

		virtual void Initialize(IGameStateDatabase* db);
		virtual bool Do(float dt, IGameStateDatabase* db);
		virtual void Destroy();
	};

}	}	}	}
