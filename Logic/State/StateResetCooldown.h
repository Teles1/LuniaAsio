#pragma once

#include "State.h"


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class ResetCooldown : public State<Player> {
		Database::Info::StateInfo::RangeType type;
		float range; ///< �����̶�� �������� �Ķ���ͷ� ����.
		uint8 targetCount; ///< ����� �ִ� ����.

	public :
		ResetCooldown(Player* owner, Database::Info::StateInfo::RangeType t, float r, uint8 cnt) : type(t), range(r), targetCount(cnt),
		   State<Player>(owner, Database::Info::StateInfo::Type::RESETCOOLDOWN, Database::Info::StateInfo::Sort::ORDINARYSTATE) {}

		bool Do(float dt, IGameStateDatabase* db);
	};

}	}	}	}
