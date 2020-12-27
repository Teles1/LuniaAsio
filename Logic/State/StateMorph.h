#pragma once

#include "State.h"


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class Morph : public State<Actor> {
		uint32 objectHash;
		Action::Action::ActionPtrList* oldActionList;//바꾸기 전에 쓰이고 있는 actionlist의 pointer

	public :
		Morph(Actor* owner, uint32 o) :
		   State<Actor>(owner, Database::Info::StateInfo::Type::MORPH, Database::Info::StateInfo::Sort::ORDINARYSTATE), objectHash(o), oldActionList(NULL) {}

		void Initialize(IGameStateDatabase* db);
		void Destroy();
	};

}	}	}	}
