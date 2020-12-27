#pragma once

#include "State.h"


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class ResetSkillGroupCooldown : public State<Player> {
		
		uint32	skillGroupHash;

	public:
		ResetSkillGroupCooldown(Player* owner, const std::wstring& skillGroupName)
			:	State<Player>(owner, Database::Info::StateInfo::Type::RESETSKILLGROUPCOOLDOWN, Database::Info::StateInfo::Sort::ORDINARYSTATE),
				skillGroupHash(StringUtil::Hash(skillGroupName.c_str()))
		{
		}

		bool Do(float dt, IGameStateDatabase* db);
	};

}	}	}	}
