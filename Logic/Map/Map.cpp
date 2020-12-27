#include "Map.h"
#include <Core/Resource/Resource.h>

namespace Lunia { namespace XRated {	namespace Logic {
	namespace Map {


		StageMap::StageMap()
		{
		}
		StageMap::~StageMap()
		{
		}

		bool StageMap::LoadMap(const Database::Info::StageInfo* info)
		{
			moveMap = &( info->GetMoveMapInfo() );
			logicalSize.SetVector( (float)moveMap->width*XRated::Constants::MapTileSizeWidth, (float)moveMap->height*XRated::Constants::MapTileSizeHeight );
			return true;
		}

		bool StageMap::IsAbletoLocate(const float3& pos, float rad, Height height)
		{
			float2 position[4];
			position[0].SetVector(pos.x, pos.z + rad);
			position[1].SetVector(pos.x + rad, pos.z);
			position[2].SetVector(pos.x, pos.z - rad);
			position[3].SetVector(pos.x - rad, pos.z);

			for (int a=0;a<4;a++) {
				int x = (int)(position[a].x / XRated::Constants::MapTileSizeWidth);
				int y = (int)(position[a].y / XRated::Constants::MapTileSizeHeight);
				if (x <= 0 || x >= moveMap->width)
					return false;
				if (y <= 0 || y >= moveMap->height)
					return false;
				if (!moveMap->LogicWalk(x,y))
					return false;
				if ( moveMap->GetHeight(x,y) > height )
					return false;
			}
			return true;
		}

	}
}	}	}