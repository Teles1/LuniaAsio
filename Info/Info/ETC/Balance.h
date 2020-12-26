#pragma once

#include <Core/GameConstants.h>

namespace Lunia {
	namespace XRated {
		namespace Database {

			class	IBalancer {
			public:
				enum ValueType {
					Vit = 0,
					Hp,
					DamageFactor,
					HealingFactor,
					AverageHp,		///	Not Nedd level Param
					AverageDamage,	///	Not Nedd level Param
				};

				virtual void	LoadData(bool bForceXml) = 0;
				virtual void	SaveData(bool bForceXml) const = 0;
				virtual	float	GetValue(XRated::Constants::ClassType classType, ValueType valueType, uint16 level = 0) = 0;
			};

			IBalancer& BalancerInstance();

		}
	}
}