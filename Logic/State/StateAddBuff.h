#pragma once

#include "State.h"


namespace Lunia { namespace XRated { namespace Logic { namespace Stat {

	class AddBuffWhenHit : public State<Actor> {
		Lunia::Math::PseudoRandomFloat* randomFloat;
		float probability;
		uint32 bundleHash;
		Database::Info::StateBundleInfo* bundleInfo;
		bool bSelf;

	public :
		AddBuffWhenHit(Actor* owner, float prob, uint32 hash, bool self)
			: State<Actor>(owner, Database::Info::StateInfo::Type::ADDBUFFWHENHIT, Database::Info::StateInfo::Sort::ATTACKSTATE)
			, randomFloat(NULL), probability(prob), bundleHash(hash), bundleInfo(NULL), bSelf(self) {}

		bool IsActivated();
		void Initialize(IGameStateDatabase*);
		void Do(Object* target, IGameStateDatabase* db);
	};

	class AddBuffWhenHitted : public State<Actor> {
		Lunia::Math::PseudoRandomFloat* randomFloat;
		float probability;
		uint32 bundleHash;
		Database::Info::StateBundleInfo* bundleInfo;
		bool bSelf;

	public :
		AddBuffWhenHitted(Actor* owner, float prob, uint32 hash, bool self)
			: State<Actor>(owner, Database::Info::StateInfo::Type::ADDBUFFWHENHITTED, Database::Info::StateInfo::Sort::ATTACKSTATE)
			, randomFloat(NULL), probability(prob), bundleHash(hash), bundleInfo(NULL), bSelf(self) {}

		bool IsActivated();
		void Initialize(IGameStateDatabase*);
		void Do(Object* target, IGameStateDatabase* db);
	};

}	}	}	}
