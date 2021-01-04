#include "StateIgnoreFreeze.h"
#include "../LogicDB/LogicDB.h"

namespace Lunia {
	namespace XRated {
		namespace Logic {
			namespace Stat {
				void IgnoreFreeze::Initialize(IGameStateDatabase* db)
				{
					switch (useState)
					{
					case 0:
						if (db->GetGameType() & XRated::Constants::GameTypes::PvpGameTypeMask)
						{
							bUse = false;
							return;
						}
						break;
					default:
						bUse = true;
						break;
					}

					if (bIgnore)
						owner->SetIgnoreFreeze(flag);
					else
						owner->RemoveIgnoreFreeze(flag);
				}

				void IgnoreFreeze::Destroy()
				{
					if (bUse)
						owner->RemoveIgnoreFreeze(flag);
				}
			}
		}
	}
}