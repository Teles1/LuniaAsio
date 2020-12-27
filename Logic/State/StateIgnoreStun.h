#pragma once

#include "State.h"


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class IgnoreStun : public State<Actor> {
		uint32 flag; //셋팅하고 싶은 항목들의 & 값
		bool bIgnore; //flag의 항목들을 셋팅할것인지 풀어버릴 것인지.
		uint8 useState;
		bool  bUse;

		uint32 old; //자신이 셋팅을 하기전의 값.

	public :
		IgnoreStun(Actor* owner, uint32 f, bool ignore, uint8 usestate)
			: State<Actor>(owner, Database::Info::StateInfo::Type::IGNORESTUN, Database::Info::StateInfo::Sort::DEFENCESTATE)
			, flag(f), bIgnore(ignore), useState(usestate), old(0), bUse(true)
		{}
		virtual ~IgnoreStun() {}

		void Initialize(IGameStateDatabase* db);
		void Destroy();
	};

}	}	}	}
