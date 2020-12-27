#pragma once

//#include "State.h"
#include "StateAttack.h"

namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class Damage : public State<Actor> {
	protected :
		float minDmg, maxDmg;
		float delay;
		float currentTime;
		Constants::DamageType dmgType;
		StatConstants::AttackEffectType::Type atkEffectType;
		int targetType;
		Lunia::XRated::Serial creatorSerial;
		uint32 creatorActionHash;
		float knockBack, makeFly, stunLength, airMakeFly, airKnockBack;
		float3 direction;

		void HitAir();
		void Stun();
		void AirStun();
		void StepBack();

		/**
			작업자 : Hak
			Air Attack Combo 관련..
		*/
		uint32 damageStateTypeUniqueSerial;			//Air Attack 분류하기 위한 ID.. 뷁!
	public :
		Damage(Actor* owner, float min, float max, float dt, Constants::DamageType type, StatConstants::AttackEffectType::Type aet
				, int tt, float knockback, float makefly, float stunlength, float airmakefly, float airknockback)
			: State<Actor>(owner, Database::Info::StateInfo::Type::DAMAGE, Database::Info::StateInfo::Sort::ORDINARYSTATE)
			, minDmg(min), maxDmg(max), delay(dt), currentTime(0), dmgType(type), atkEffectType(aet), targetType(tt)
			, knockBack(knockback), makeFly(makefly), stunLength(stunlength), airMakeFly(airmakefly), airKnockBack(airknockback) {}
		virtual ~Damage() {}

		void Initialize(Projectile* projectile, IGameStateDatabase* db, uint32 id = 0);
		virtual bool Do(float dt, IGameStateDatabase* db);
	};

}	}	}	}
