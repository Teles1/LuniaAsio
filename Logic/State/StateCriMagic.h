#pragma once

#include "State.h"


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class CriMagic : public State<Actor> {
		float probability;

	public :
		CriMagic(Actor* owner, float p)
			: State<Actor>(owner, Database::Info::StateInfo::Type::CRIMAGIC, Database::Info::StateInfo::Sort::ATTACKSTATE)
			, probability(p)
		{}
		virtual ~CriMagic() {}

		virtual void Initialize(IGameStateDatabase* db);
		virtual void Destroy();
	};

}	}	}	}
