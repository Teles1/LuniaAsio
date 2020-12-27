#pragma once

#include <Core/Utils/Math/Math.h>

namespace Lunia { namespace XRated { namespace Logic { namespace AI {

	void DecideDirection(const float3& sourcePos, const float3& targetPos, uint32*, bool backward = false);
	float3 DirectionToFloat3(uint32 dir);

}	}	}	}