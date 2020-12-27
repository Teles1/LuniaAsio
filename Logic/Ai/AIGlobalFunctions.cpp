#include "AIGlobalFunctions.h"
#include <Core/GameConstants.h>

namespace Lunia { namespace XRated { namespace Logic { namespace AI {

	void DecideDirection(const float3& sourcePos, const float3& targetPos, uint32* direction, bool backward)
	{
		float3 dir = targetPos - sourcePos;
		if (backward) dir *= -1;
		if (dir.x == 0) dir.x = 0.000001f;// devide by zero

		if (dir.x >= 0 && dir.z > 0) {
			if (dir.x < dir.z) {
				if (dir.z/dir.x > 2.4f) {
					direction[0] = 8;
					direction[1] = 9;
				} else {
					direction[0] = 9;
					direction[1] = 8;
				}
				direction[2] = 7;
				direction[3] = 6;
				direction[4] = 4;
				direction[5] = 3;
			} else {
				if (dir.z/dir.x < 0.414213f) {
					direction[0] = 6;
					direction[1] = 9;
				} else {
					direction[0] = 9;
					direction[1] = 6;
				}
				direction[2] = 3;
				direction[3] = 8;
				direction[4] = 2;
				direction[5] = 7;
			}
		} else if (dir.x >= 0 && dir.z <= 0) {
			if (dir.x >= -dir.z) {
				if (-dir.z/dir.x < 0.414213f) {
					direction[0] = 6;
					direction[1] = 3;
				} else {
					direction[0] = 3;
					direction[1] = 6;
				}
				direction[2] = 9;
				direction[3] = 2;
				direction[4] = 8;
				direction[5] = 1;
			} else {
				if (-dir.z/dir.x > 2.4f) {
					direction[0] = 2;
					direction[1] = 3;
				} else {
					direction[0] = 3;
					direction[1] = 2;
				}
				direction[2] = 1;
				direction[3] = 6;
				direction[4] = 4;
				direction[5] = 9;
			}
		} else if (dir.x < 0 && dir.z < 0) {
			if (dir.x > dir.z) {
				if (dir.z/dir.x > 2.4f) {
					direction[0] = 2;
					direction[1] = 1;
				} else {
					direction[0] = 1;
					direction[1] = 2;
				}
				direction[2] = 4;
				direction[3] = 3;
				direction[4] = 6;
				direction[5] = 7;
			} else {
				if (dir.z/dir.x < 0.414213f) {
					direction[0] = 4;
					direction[1] = 1;
				} else {
					direction[0] = 1;
					direction[1] = 4;
				}
				direction[2] = 7;
				direction[3] = 2;
				direction[4] = 8;
				direction[5] = 3;
			}
		} else {
			if (-dir.x >= dir.z) {
				if (dir.z/(-dir.x) < 0.414213f) {
					direction[0] = 4;
					direction[1] = 7;
				} else {
					direction[0] = 7;
					direction[1] = 4;
				}
				direction[2] = 1;
				direction[3] = 8;
				direction[4] = 2;
				direction[5] = 9;
			} else {
				if (dir.z/(-dir.x) < 2.4f) {
					direction[0] = 7;
					direction[1] = 8;
				} else {
					direction[0] = 8;
					direction[1] = 7;
				}
				direction[2] = 4;
				direction[3] = 9;
				direction[4] = 6;
				direction[5] = 1;
			}
		}
	}

	float3 DirectionToFloat3(uint32 dir)
	{
		switch ( dir )  {
		case 1 :
			return Constants::DirectionF::LeftDown;
		case 2 :
			return Constants::DirectionF::Down;
		case 3 :
			return Constants::DirectionF::RightDown;
		case 4 :
			return Constants::DirectionF::Left;
		case 6 :
			return Constants::DirectionF::Right;
		case 7 :
			return Constants::DirectionF::LeftUp;
		case 8 :
			return Constants::DirectionF::Up;
		case 9 :
			return Constants::DirectionF::RightUp;
		default :
			break;
			//throw( Lunia_EXCEPTION(( "[AIGlobalFunctions::DirectionToFloat3] Unknown direction [{}].", dir )) );
		}
		return Constants::DirectionF::Left;
	}

}	}	}	}
