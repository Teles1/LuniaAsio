#pragma once

#include <Core/Utils/Math/Math.h>
#include <Info/Info.h>

namespace Lunia { namespace XRated {	namespace Logic {
	namespace Map {

		class StageMap {
		public :
			enum Height {
				FIELD = 0,
				WATER = 2,
				FENCE = 4,
				UNPASSABLE = 7
			};

			const static int TILESIZE_X = 16;
			const static int TILESIZE_Y = 16;

		protected :
			const Database::Info::StageInfo::MoveMap* moveMap;

			float2 logicalSize;

		public :
			StageMap();
			virtual ~StageMap();

			virtual bool LoadMap(const Database::Info::StageInfo*);
			virtual bool IsAbletoLocate(const float3& pos, float radius, Height height);
			float GetWidth() { return ((float)moveMap->width)*XRated::Constants::MapTileSizeWidth; }
			float GetHeight() { return ((float)moveMap->height)*XRated::Constants::MapTileSizeHeight; }
			const float2& GetLogicalSize() const { return logicalSize; }
		};

}	}	}	}