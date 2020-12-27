#pragma once

#include "State.h"


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class Reflection : public State<Object> {
	protected :
		float damage;

	public :
		Reflection(Object* owner, float dmg) : damage(dmg),
			State<Object>(owner, Database::Info::StateInfo::Type::REFLECTION, Database::Info::StateInfo::Sort::DEFENCESTATE) {}
		virtual ~Reflection() {}

		   const float& GetDamage() const { return damage; }
	};

}	}	}	}
