#pragma once

#include "State.h"


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	// dingpong. ������Ʈ Ÿ���̱� ������onwer �� Actor �� ���� �� �ֵ��� �Ѵ�.
	class CreateObjectProjectile : public State<Actor> {
		uint32 name;
		uint16 fireAngle, type, toMyTarget;
		float2 position;
		float minDmg, maxDmg;
		float createTime;

	public :
		CreateObjectProjectile(Actor* owner, uint32 n, float2 pos, uint16 angle, uint16 tp, float min, float max, uint16 toTarget, float time) :
			State<Actor>(owner, Database::Info::StateInfo::Type::CREATEPROJECTILE, Database::Info::StateInfo::Sort::ORDINARYSTATE),
			name(n), position(pos), fireAngle(angle), type(tp), minDmg(min), maxDmg(max), toMyTarget(toTarget), createTime(time) {}
		virtual ~CreateObjectProjectile() {}

		void Initialize(IGameStateDatabase*);
		bool Do(float dt, IGameStateDatabase* db);
	};

}	}	}	}
