#pragma once
#include <Core/Serializer/Serializer.h>
#include <Info/Info/GlobalInfo.h>
#include <Core/GameConstants.h>

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				struct NonPlayerModifierInfo
				{
					struct ModifierInfo
					{
						float AtkMultiply;
						float HpMultiply;
						float XpMultiply;
						float HitIgnoreMultiply;
					public:
						virtual void Serialize(Serializer::IStreamWriter& out) const;
						virtual void Deserialize(Serializer::IStreamReader& in);

						ModifierInfo() : AtkMultiply(0.0f),
							HpMultiply(1.0f),
							XpMultiply(1.0f),
							HitIgnoreMultiply(1.0f)
						{}
					};

					typedef std::map< uint8 /*userCnt*/, ModifierInfo> ModifierInfoTable;
					std::map< uint8 /*NonPlayerData::NpcType*/, ModifierInfoTable > ModifierInfos;

					const NonPlayerModifierInfo::ModifierInfo* GetModifierInfo(NonPlayerData::NpcType type, int userCnt) const;

					ModifierInfo	defaultModifierInfo;
				public:
					virtual void Serialize(Serializer::IStreamWriter& out) const;
					virtual void Deserialize(Serializer::IStreamReader& in);
				};

				struct NonplayerLvModifierInfo
				{
					struct ModifierInfo
					{
						std::wstring BasicStat;
						uint32 Lv;
						uint32 Exp;
					public:
						virtual void Serialize(Serializer::IStreamWriter& out) const;
						virtual void Deserialize(Serializer::IStreamReader& in);

						ModifierInfo() : BasicStat(L""),
							Lv(0),
							Exp(0)
						{}
					};

					typedef std::map< uint32 /*Before NPC Lv*/, ModifierInfo /*After NPC Lv*/> LvModifierInfoTable;
					std::map< uint8 /*StageInfo::RaidStageDifficulty*/, LvModifierInfoTable > LvModifierInfos;

					const NonplayerLvModifierInfo::ModifierInfo* GetLv(uint8 stageDifficulty, uint32 beforeNpcLv) const;
				public:
					virtual void Serialize(Serializer::IStreamWriter& out) const;
					virtual void Deserialize(Serializer::IStreamReader& in);
				};
			}
		}
	}
}
