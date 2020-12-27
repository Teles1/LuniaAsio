#pragma once

#include "State.h"


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class BlockUsingSkill : public State<Actor> {
	public :
		BlockUsingSkill(Actor* owner) :
		   State<Actor>(owner, Database::Info::StateInfo::Type::BLOCKUSINGSKILL, Database::Info::StateInfo::Sort::ATTACKSTATE) {}
	};

}}}}
