#pragma once

#include "State.h"


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class Down : public State<Actor> {
	public :
		Down(Actor* owner) : State<Actor>(owner, Database::Info::StateInfo::Type::DOWN,
			Database::Info::StateInfo::Sort::DEFENCESTATE) {}
		~Down() {}

		//void Initialize(IGameStateDatabase*) { owner->SetOverlappable(true); }
		//void Destroy() { owner->SetOverlappable(false); }
	};

	class Passable : public State<Actor> {
	public :
		Passable(Actor* owner) : State<Actor>(owner, Database::Info::StateInfo::Type::PASSABLE,
			Database::Info::StateInfo::Sort::DEFENCESTATE) {}
		~Passable() {}

		void Initialize(IGameStateDatabase*) { owner->SetOverlappable(); }
	};

}	}	}	}
