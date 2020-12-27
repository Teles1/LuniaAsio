#pragma once

#include "State.h"


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	// TODO : 프로젝타일에만 쓰인다.
	class CreateProjectile : public State<Projectile> {
		uint32 name;
		uint16 fireAngle, type, toMyTarget;
		float2 position;
		float minDmg, maxDmg;
		float createTime;

	public :
		CreateProjectile(Projectile* owner, uint32 n, float2 pos, uint16 angle, uint16 tp, float min, float max, uint16 toTarget, float time) :
			State<Projectile>(owner, Database::Info::StateInfo::Type::CREATEPROJECTILE, Database::Info::StateInfo::Sort::ORDINARYSTATE),
			name(n), position(pos), fireAngle(angle), type(tp), minDmg(min), maxDmg(max), toMyTarget(toTarget), createTime(time) {}
		virtual ~CreateProjectile() {}

		void Initialize(IGameStateDatabase*);
		bool Do(float dt, IGameStateDatabase* db);
	};

}	}	}	}
