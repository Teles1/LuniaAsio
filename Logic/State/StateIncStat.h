#pragma once

#include "State.h"


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class IncStat : public State<Actor> {
	public :
		enum Category 
		{
			MAXHP, 
			MAXMP, 
			ATKMAX, 
			ATKMIN, 
			DEF, 
			STR, 
			INT, 
			DEX, 
			VIT, 
			ATKSPD
		};
		enum ChargeType {
			RATE, AMOUNT
		};
		enum EnableState {
            NEEDINIT = 0, ENABLE, DISABLE, 
		};
	protected :
		Category category;
		ChargeType chargeType;
		float amount;
		int stateEnable;
		bool bFill;

	public :
		IncStat(Actor* owner, Category cat, ChargeType ct, float a, bool fill)
			: State<Actor>(owner, Database::Info::StateInfo::Type::INCSTAT, Database::Info::StateInfo::Sort::DEFENCESTATE),
			category(cat), chargeType(ct), amount(a), bFill(fill), stateEnable(NEEDINIT) { }
		virtual ~IncStat() {}

		virtual void Initialize(IGameStateDatabase*);//�����Ǹ鼭 object�� ������ ��ġ�� �κ��� ó��.
		virtual void Destroy();		//�Ҹ�Ǹ鼭 object�� ������ ��ġ�� �κ��� ó��.
		virtual void Enable(bool enable);
	};

}	}	}	}
