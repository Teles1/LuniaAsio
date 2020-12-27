#pragma once

#include "State.h"


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class CreateObject : public State<Actor> {
	protected :
		uint32 name;
		float2 startPos;
		int hp;
		float duration;

	public :
		CreateObject(Actor* owner, uint32 n, float2 pos, int h, float dur) :
		   State<Actor>(owner, Database::Info::StateInfo::Type::CREATEOBJECT, Database::Info::StateInfo::Sort::ORDINARYSTATE),
		   name(n), startPos(pos), hp(h), duration(dur) {}
		virtual ~CreateObject() {}

		bool Do(float dt, IGameStateDatabase* db);
	};

}	}	}	}
