#pragma once

#include "State.h"


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	/** �̳��� �ø��ų� ���̱��ص� ���߿� ���󺹱͸� �Ƚ�Ų��.
	���� ���������� �ö󰡴� max��ġ�� ��� �̳��� ���� �ʰ� INCSTAT state�� ����Ѵ�. */
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
