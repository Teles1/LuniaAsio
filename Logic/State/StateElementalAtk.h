#pragma once

#include "State.h"


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class ElementalAtk : public State<Actor> {
	protected :
		Constants::DamageType element;

	public :
		ElementalAtk(Actor* owner, Constants::DamageType type)
			: State<Actor>(owner, Database::Info::StateInfo::Type::ELEMENTALATK, Database::Info::StateInfo::Sort::ORDINARYSTATE)
			, element(type) {}
			virtual ~ElementalAtk() {}

			virtual void Initialize(IGameStateDatabase* db);
			virtual void Destroy();
			Constants::DamageType GetElement(){return element;}
	};

}	}	}	}
