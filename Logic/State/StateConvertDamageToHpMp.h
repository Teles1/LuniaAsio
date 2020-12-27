#pragma once

#include "State.h"


namespace AllM { namespace XRated { namespace Logic { namespace Stat {

	class ConvertDamageToHpMp : public State<Actor>
	{
	public:
		enum RecoveryType	{ Hp, Mp };
		enum TargetType		{ Owner, Other };
	
	private:

		XRated::Serial otherActorSerial;
		
		float rate;
		float damage;

		RecoveryType recoveryType;

		TargetType checkTarget;
		TargetType affectTarget;

	public:
		ConvertDamageToHpMp(Actor* owner, RecoveryType recoveryType, TargetType checkTarget, TargetType affectTarget, float rate)
			:	State<Actor>(owner, Database::Info::StateInfo::Type::CONVERTDMGTOHPMP, Database::Info::StateInfo::Sort::ORDINARYSTATE),
				otherActorSerial(0), rate(rate), damage(0.0f), recoveryType(recoveryType), checkTarget(checkTarget), affectTarget(affectTarget)
		{
		}

		void Do(IGameStateDatabase* db);

		TargetType GetCheckTargetType() const { return checkTarget; }

		void SetParamValue(XRated::Serial otherActorSerial, float damage);
	};

}	}	}	}
