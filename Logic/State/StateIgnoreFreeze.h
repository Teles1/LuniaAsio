#pragma once

#include "State.h"


namespace Lunia {
	namespace XRated {
		namespace Logic {
			namespace Stat {

				class IgnoreFreeze : public State<Actor> {
					uint32 flag;
					bool bIgnore;
					uint8 useState;
					bool  bUse;

					uint32 old;

				public:
					IgnoreFreeze(Actor* owner, uint32 f, bool ignore, uint8 usestate)
						: State<Actor>(owner, Database::Info::StateInfo::Type::IGNOREFREEZE, Database::Info::StateInfo::Sort::DEFENCESTATE)
						, flag(f), bIgnore(ignore), useState(usestate), old(0), bUse(true)
					{}
					virtual ~IgnoreFreeze() {}

					void Initialize(IGameStateDatabase* db);
					void Destroy();
				};

			}
		}
	}
}
