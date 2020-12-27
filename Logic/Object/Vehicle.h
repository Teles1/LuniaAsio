#pragma once
#include "Actor.h"

namespace Lunia { namespace XRated {	namespace Logic {

	class Vehicle : public Actor {
	public :
		Vehicle()
			: Actor(Lunia::XRated::Constants::ObjectType::Vehicle) {}

	};

}	}	}
