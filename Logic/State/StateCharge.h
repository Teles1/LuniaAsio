#pragma once

#include "State.h"


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	/** 이놈은 늘리거나 줄이긴해도 나중에 원상복귀를 안시킨다.
	따라서 아이템으로 올라가는 max수치의 경우 이놈을 쓰지 않고 INCSTAT state를 사용한다. */
	class Charge : public State<Actor> {
	public :
		enum Category {
			HP, MP, DASH
		};
		enum ChargeType {
			RATE, AMOUNT
		};
	protected :
		Category category;
		ChargeType chargeType;
		float amount;

	public :
		Charge(Actor* owner, Category cat, ChargeType ct, float a)
			: State<Actor>(owner, Database::Info::StateInfo::Type::CHARGE, Database::Info::StateInfo::Sort::ORDINARYSTATE),
				category(cat), chargeType(ct), amount(a) {}
		virtual ~Charge() {}

		void Initialize(IGameStateDatabase* db);
		bool Do(float dt, IGameStateDatabase* db);
	};

}	}	}	}
