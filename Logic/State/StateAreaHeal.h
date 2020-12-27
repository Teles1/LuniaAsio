#pragma once

#include "State.h"


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class AreaHeal : public State<Object> {
	public :
		enum FillType { Hp=0, Mp, HpRate, MpRate };

	private :
		float range;
		float regenRate;
		FillType fillType;
		Constants::AllianceType allianceType;

	public :
		AreaHeal(Object* owner, float r, float rate, FillType ft, Constants::AllianceType type);
		virtual ~AreaHeal() {}

		bool Do(float dt, IGameStateDatabase* db);
	};

}	}	}	}
