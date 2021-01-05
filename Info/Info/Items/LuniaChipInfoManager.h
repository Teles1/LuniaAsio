#pragma once
#include <Core/Serializer/Serializer.h>
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				class LuniaChipInfoManager
				{
					std::map< uint32 /*Hash*/, int /*ChipType*/ > infos;
					typedef std::map< uint32 /*Hash*/, int /*ChipType*/ >::iterator ChipIter;

				public:
					bool IsChip(uint32 Hash);
					int GetChipType(uint32 Hash);
					uint32 GetChipHash(int ChipType);

					void Load(bool xml);
					void Save(bool xml);
					void Init(bool xml);
				};
			}
		}
	}
}