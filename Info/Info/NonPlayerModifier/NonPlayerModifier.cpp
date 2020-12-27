#pragma once
#include "NonPlayerModifier.h"

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				void NonPlayerModifierInfo::ModifierInfo::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"Database::Info::NonPlayerModifierInfo::ModifierInfo");
					out.Write(L"AtkMultiply", AtkMultiply);
					out.Write(L"HpMultiply", HpMultiply);
					out.Write(L"XpMultiply", XpMultiply);
					out.Write(L"HitIgnoreMultiply", HitIgnoreMultiply);
				}

				void NonPlayerModifierInfo::ModifierInfo::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"Database::Info::NonPlayerModifierInfo::ModifierInfo");
					in.Read(L"AtkMultiply", AtkMultiply, 0.0f);
					in.Read(L"HpMultiply", HpMultiply, 0.0f);
					in.Read(L"XpMultiply", XpMultiply, 0.0f);
					in.Read(L"HitIgnoreMultiply", HitIgnoreMultiply, 1.0f);
				}


				const NonPlayerModifierInfo::ModifierInfo* NonPlayerModifierInfo::GetModifierInfo(NonPlayerData::NpcType type, int userCnt) const
				{
					std::map< uint8, ModifierInfoTable >::const_iterator iter = ModifierInfos.find(static_cast<uint8>(type));
					if (iter != ModifierInfos.end())
					{
						ModifierInfoTable::const_iterator iter2 = iter->second.find(static_cast<uint8>(userCnt));
						if (iter2 != iter->second.end())
						{
							return &iter2->second;
						}
						else
						{
							if (!iter->second.empty())
							{
								--iter2;
								return &iter2->second;
							}
						}
					}

					return &defaultModifierInfo;
				}

				void NonPlayerModifierInfo::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"Database::Info::NonPlayerModifierInfo");
					out.Write(L"ModifierInfos", ModifierInfos);
				}

				void NonPlayerModifierInfo::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"Database::Info::NonPlayerModifierInfo");
					in.Read(L"ModifierInfos", ModifierInfos);
				}

				void NonplayerLvModifierInfo::ModifierInfo::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"Database::Info::NonplayerLvModifierInfo::ModifierInfo");
					out.Write(L"BasicStat", BasicStat);
					out.Write(L"Lv", Lv);
					out.Write(L"Exp", Exp);
				}

				void NonplayerLvModifierInfo::ModifierInfo::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"Database::Info::NonplayerLvModifierInfo::ModifierInfo");
					in.Read(L"BasicStat", BasicStat, std::wstring(L""));
					in.Read(L"Lv", Lv, static_cast<uint32>(1));
					in.Read(L"Exp", Exp, static_cast<uint32>(1));
				}

				const NonplayerLvModifierInfo::ModifierInfo* NonplayerLvModifierInfo::GetLv(uint8 stageDifficulty, uint32 beforeNpcLv) const
				{
					std::map< uint8, LvModifierInfoTable >::const_iterator iter = LvModifierInfos.find(stageDifficulty);
					if (iter != LvModifierInfos.end())
					{
						LvModifierInfoTable::const_iterator iter2 = iter->second.find(beforeNpcLv);
						if (iter2 != iter->second.end())
						{
							return &iter2->second;
						}
					}

					return NULL;
				}

				void NonplayerLvModifierInfo::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"Database::Info::NonplayerLvModifierInfo");
					out.Write(L"LvModifierInfos", LvModifierInfos);
				}

				void NonplayerLvModifierInfo::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"Database::Info::NonplayerLvModifierInfo");
					in.Read(L"LvModifierInfos", LvModifierInfos);
				}
			}
		}
	}
}
