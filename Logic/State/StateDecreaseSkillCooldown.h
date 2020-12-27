#pragma once

#include "State.h"


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class DecreaseSkillCooldown : public State<Player> {
		
		uint32	skillGroupHash;
		float	amount;

	public:
		DecreaseSkillCooldown(Player* owner, const std::wstring& skillGroupName, float amount)
			:	State<Player>(owner, Database::Info::StateInfo::Type::DECREASESKILLCOOLDOWN, Database::Info::StateInfo::Sort::ORDINARYSTATE),
				skillGroupHash(StringUtil::Hash(skillGroupName.c_str())), amount(amount)
		{
		}

		void	Initialize(IGameStateDatabase*);
		void	Destroy();
	};

}	}	}	}
