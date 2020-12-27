#pragma once

#include "State.h"


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class CriRush : public State<Actor> {
	protected :

	public :
		CriRush(Actor* owner) : State<Actor>(owner, Database::Info::StateInfo::Type::CRIRUSH, Database::Info::StateInfo::Sort::ATTACKSTATE) {}
		virtual ~CriRush() {}

		virtual void Initialize(IGameStateDatabase* db);
		virtual void Destroy();
	};

}	}	}	}
