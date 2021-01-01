#pragma once
#include "NonPlayerModifier.h"

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				class NonPlayerModifierTable
				{
				public:
					const NonPlayerModifierInfo::ModifierInfo* GetModifierInfo(NonPlayerData::NpcType type, int userCnt, int nonPlayerLevel) const;
					const NonplayerLvModifierInfo::ModifierInfo* GetLv(uint8 stageDifficulty, uint32 beforeNpcLv) const;

					void Init(bool xml);
					void Save(bool xml);
				private:
					std::map<int /*Level*/, NonPlayerModifierInfo > nonPlayerModifierInfos;
					NonplayerLvModifierInfo	nonPlayerLvModifierInfo;
				};
			}
		}
	}
}
