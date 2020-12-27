#pragma once

#include "State.h"


namespace AllM { namespace XRated { namespace Logic { namespace Stat {

	class BlockProjectile : public State<Object> {
	public :
		BlockProjectile(Object* owner) :
		   State<Object>(owner, Database::Info::StateInfo::Type::BLOCKPROJECTILE, Database::Info::StateInfo::Sort::ORDINARYSTATE) {}
		virtual ~BlockProjectile() {}

		bool Do(float dt, IGameStateDatabase* db);
	};

}	}	}	}
