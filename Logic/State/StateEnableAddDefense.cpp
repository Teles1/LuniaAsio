#include "StateEnableAddDefense.h"
#include "../LogicDB/LogicDB.h"
using namespace Lunia::StringUtil;
using namespace Lunia::Math;
using namespace Lunia::XRated::Database;

namespace Lunia {
	namespace XRated {
		namespace Logic {
			namespace Stat {
				void EnableAddDefense::Initialize(IGameStateDatabase*)
				{
					Enable(true);
				}
				void EnableAddDefense::Destroy()
				{
					Enable(false);
				}
				void EnableAddDefense::Enable(bool enable)
				{
					if (enable)
					{
						if (stateEnable == ENABLE)
						{
							return;
						}

						stateEnable = ENABLE;

						if (chargeType == ChargeType::AMOUNT)
							owner->AddAc(amount);
						else
							owner->AddAcMul(amount);
						owner->RecalculateAc(owner->GetType() == Constants::ObjectType::NonPlayer ? true : false);
					}
					else
					{
						if (stateEnable == DISABLE || stateEnable == NEEDINIT)
						{
							return;
						}

						stateEnable = DISABLE;

						if (chargeType == ChargeType::AMOUNT)
							owner->RemoveAc(amount);
						else
							owner->RemoveAcMul(amount);
							
						owner->RecalculateStatus(owner->GetType() == Constants::ObjectType::NonPlayer ? true : false);
						//Lunia_IMPORTANT(("[State Enable] Category : {}, Amount : {}, ", category, (int)-amount));
					}
				}
			}
		}
	}
}