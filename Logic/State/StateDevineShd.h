/*
    Copyright (c) 2000-, Lunia Corp. All rights reserved.
        http://www.Lunia.co.kr
            tel:82-2-888-1555
                fax:82-2-888-4414
                    mailTo:webmaster@Lunia.co.kr
*/
#pragma once

#include "State.h"
#include "StateInvincible.h"

namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class DevineShd : public State<Actor> {
	protected :
		float duration;
		//Invincible* stateInvincible;

	public :
		DevineShd(Actor* owner, float dur)
			: State<Actor>(owner, Database::Info::StateInfo::Type::DEVINESHD, Database::Info::StateInfo::Sort::ORDINARYSTATE)
			, duration(dur)/*, stateInvincible( NULL )*/
		{}
		virtual ~DevineShd() {}

		virtual void Initialize(IGameStateDatabase* db);
		virtual bool Do(float dt, IGameStateDatabase* db);
		virtual void Destroy();
		void Remove();
	};

}	}	}	}
