#pragma once
#include <Core/Serializer/Serializer.h>
#include <Info/Info/GlobalInfo.h>
#include <Core/GameConstants.h>

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				struct StatusInfo
				{
					uint16 Level;
					//uint32 MinimalXp; ///> base experiance point to be this level

					/* primary status */
					int Strength; ///> affects minimum and maximum damage
					int Intelligence; ///> affects maximum mp
					int Dextrity; ///> affects reducing damage, maximum damage and regeneration rate of hp and mp
					int Vitality; ///> affects maximum hp and armor class

					//float regenHP, regenMP;

					float Hp, Mp;

					//public :
					//	void CalculateSecondStat();
				};
			}
		}
	}
}
