#pragma once

#include "State.h"


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class ChangeAction : public State<Actor> {
		Database::Info::StateInfo::RangeType type;
		Constants::AllianceType applyTo;
		uint32 actionHash, targetHash;
		bool bForce;
		float range;

	public :
		ChangeAction(Actor* owner, Database::Info::StateInfo::RangeType tp, Constants::AllianceType to, float r, uint32 action, uint32 target, bool force) :
		   State<Actor>(owner, Database::Info::StateInfo::Type::CHANGEACTION, Database::Info::StateInfo::Sort::ORDINARYSTATE),
		   type(tp), applyTo(to), range(r), actionHash(action), targetHash(target), bForce(force)  {}

		void Initialize(IGameStateDatabase* db);
		void Destroy();
	};

}	}	}	}
