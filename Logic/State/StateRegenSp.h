#pragma once

#include "State.h"


namespace AllM { namespace XRated { namespace Logic { namespace Stat {

	class RegenSp : public State<Player> {
	protected :
		float speed;
		float elapsed;
		float paused;
		const float tick;

	public :
		RegenSp(Player* owner, float s) :
		   State<Player>(owner, Database::Info::StateInfo::Type::REGENSP, Database::Info::StateInfo::Sort::ORDINARYSTATE),
		   speed(s), elapsed(0), paused(0), tick(AllM::XRated::Constants::Tick*2) {}
		RegenSp& operator= (const RegenSp& state) {
			speed = state.speed;
			elapsed = state.elapsed;
			paused = state.paused;
            return *this;
		}
		virtual ~RegenSp() {}

		void PauseRegen() {paused = AllM::XRated::Constants::Tick*4;} 
		virtual bool Do(float dt, IGameStateDatabase* db);
	};

}	}	}	}
