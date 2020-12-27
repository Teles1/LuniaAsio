#pragma once

#include "State.h"


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class ResetCooldown : public State<Player> {
		Database::Info::StateInfo::RangeType type;
		float range; ///< 원형이라면 레인지를 파라메터로 받자.
		uint8 targetCount; ///< 대상의 최대 숫자.

	public :
		ResetCooldown(Player* owner, Database::Info::StateInfo::RangeType t, float r, uint8 cnt) : type(t), range(r), targetCount(cnt),
		   State<Player>(owner, Database::Info::StateInfo::Type::RESETCOOLDOWN, Database::Info::StateInfo::Sort::ORDINARYSTATE) {}

		bool Do(float dt, IGameStateDatabase* db);
	};

}	}	}	}
