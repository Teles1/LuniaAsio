#pragma once

#include "State.h"


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class InventFame : public State<Player> {
	protected :
		std::wstring text;

	public :
		InventFame(Player* owner, std::wstring t) : text(t),
			State<Player>(owner, Database::Info::StateInfo::Type::INVENTFAME, Database::Info::StateInfo::Sort::NORMAL) {}
		virtual ~InventFame() {}

		virtual void Initialize(IGameStateDatabase* db);
		virtual void Destroy();
	};

}	}	}	}