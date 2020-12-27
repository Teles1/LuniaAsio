#pragma once

#include "State.h"
#include "../Object/Projectile.h"

namespace Lunia { namespace XRated {	namespace Logic { namespace Stat {

	class Flied : public State<Object> { public : Flied(Object* owner) : State<Object>(owner, Database::Info::StateInfo::Type::FLIED, Database::Info::StateInfo::Sort::DEFENCESTATE) {} ~Flied() {} };
	class Ghost : public State<Actor> {public : Ghost(Actor* owner) : State<Actor>(owner, Database::Info::StateInfo::Type::GHOST, Database::Info::StateInfo::Sort::DEFENCESTATE) {} ~Ghost() {}};
	class Stun : public State<Object> {public : Stun(Object* owner) : State<Object>(owner, Database::Info::StateInfo::Type::STUN, Database::Info::StateInfo::Sort::DEFENCESTATE) {} ~Stun() {} };
	class Zombie : public State<Actor> {public : Zombie(Actor* owner) : State<Actor>(owner, Database::Info::StateInfo::Type::ZOMBIE, Database::Info::StateInfo::Sort::DEFENCESTATE) {} ~Zombie() {}};


	/* statebundle에 optionalStates를 넣어려다 일단 스톱.
	template <class T>
	IState* CreateState(Database::Info::StateInfo::Type type, T* owner, StateParams* params, std::vector<Database::Info::ActionInfo::State>* states, Lunia::XRated::Serial creator = 0);
	template <class T>
	IState* CreateState(Database::Info::StateInfo::Type type, T* owner, StateParams* params, std::vector<Database::Info::StateBundleInfo::State>* states, Lunia::XRated::Serial creator = 0);
	*/
	//Create state template
	template<class T> IState* CreateState(Database::Info::StateInfo::Type type, T* owner, StateParams* params, std::vector<Database::Info::ActionInfo::State>* states, Lunia::XRated::Serial creator = 0);
	IState* CreateState(Database::Info::StateInfo::Type type, Projectile* projectile, Actor* target, StateParams* params,Lunia::XRated::Serial creator = 0);
	/// 안쓰이네여? 2008.02.13 by Yong-Eun, Jung
	//IState* CreateState(Database::Info::StateInfo::Type type, std::vector<float>& params,Lunia::XRated::Serial creator = 0);

	IState* CreateState(const IState*, Object* owner, Lunia::XRated::Serial creator = 0); //Template 으로 부터 복사생성함수

}	}	}	}
