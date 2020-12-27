#pragma once

#include "State.h"

namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class IgnoreCriticalHit : public State<Actor> {
	protected :
		float probability;

	public :
		IgnoreCriticalHit(Actor* owner, float p)
			: State<Actor>(owner, Database::Info::StateInfo::Type::IGNORECRITICALHIT, Database::Info::StateInfo::Sort::DEFENCESTATE)
			, probability(p) {}
		virtual ~IgnoreCriticalHit() {}

		virtual void Initialize(IGameStateDatabase* db);
		virtual void Destroy();
	};

}}}}
