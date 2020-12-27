#pragma once

#include "State.h"


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class RegenHp : public State<Actor> {
	public :
		enum ChargeType {
			RATE, AMOUNT
		};
	protected :
		ChargeType chargeType;
		float speed;
		float elapsed;
		const float tick;
		bool isRegenWhenZero;		///< hp가 0일때 멈출 것인가.

	public :
		RegenHp(Actor* owner, ChargeType ct, float s, bool isRegenWhenZeroParam)
			: State<Actor>(owner, Database::Info::StateInfo::Type::REGENHP, Database::Info::StateInfo::Sort::ORDINARYSTATE),
			chargeType(ct), speed(s), elapsed(0), tick(Lunia::XRated::Constants::Tick*2),isRegenWhenZero(isRegenWhenZeroParam) {}
		virtual ~RegenHp() {}
		void operator= (RegenHp&) {}

		virtual void Initialize(IGameStateDatabase* db);
		virtual bool Do(float dt, IGameStateDatabase* db);
	};

	class RegenMp : public State<Actor> {
	public :
		enum ChargeType {
			RATE, AMOUNT
		};
	protected :
		ChargeType chargeType;
		float speed;
		float elapsed;
		const float tick;
		bool isRegenWhenZero;		///< hp가 0일때 멈출 것인가.
		//float tmp, tmp2;

	public :
		RegenMp(Actor* owner, ChargeType ct, float s, bool isRegenWhenZeroParam)
			: State<Actor>(owner, Database::Info::StateInfo::Type::REGENMP, Database::Info::StateInfo::Sort::ORDINARYSTATE),
			chargeType(ct), speed(s), elapsed(0), tick(Lunia::XRated::Constants::Tick*2),isRegenWhenZero(isRegenWhenZeroParam) { /*tmp = tmp2 = 0;*/ }
		virtual ~RegenMp() {}
		void operator= (RegenMp&) {}

		virtual void Initialize(IGameStateDatabase* db);
		virtual bool Do(float dt, IGameStateDatabase* db);
		virtual void Destroy();
	};

}	}	}	}
