#pragma once
#include "NonPlayerModifierManager.h"

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				const NonPlayerModifierInfo::ModifierInfo* NonPlayerModifierTable::GetModifierInfo(NonPlayerData::NpcType type, int userCnt, int nonPlayerLevel) const
				{
					std::map<int, NonPlayerModifierInfo >::const_iterator selectIter = nonPlayerModifierInfos.begin();
					int selectLevel = 0;

					std::map< int, NonPlayerModifierInfo >::const_iterator iter = nonPlayerModifierInfos.begin();
					while (iter != nonPlayerModifierInfos.end())
					{
						if (nonPlayerLevel >= iter->first && iter->first > selectLevel)
						{
							selectIter = iter;
							selectLevel = iter->first;
						}
						else
							break;

						++iter;
					}

					if (selectIter != nonPlayerModifierInfos.end())
					{
						return selectIter->second.GetModifierInfo(type, userCnt);
					}

					return NULL;
				}

				const NonplayerLvModifierInfo::ModifierInfo* NonPlayerModifierTable::GetLv(uint8 stageDifficulty, uint32 beforeNpcLv) const
				{
					return nonPlayerLvModifierInfo.GetLv(stageDifficulty, beforeNpcLv);
				}

				void NonPlayerModifierTable::Load(bool xml)
				{
					if (xml == true)
					{
						{
							Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateSerializerXmlStreamReader(L"Database/NPC/ModifierInfo/NPCStatusModifierInfos.xml");
							reader->Read(L"NonPlayerModifierInfoTable", nonPlayerModifierInfos);
						}

						{
							Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateSerializerXmlStreamReader(L"Database/NPC/ModifierInfo/NPCLvModifierInfos.xml");
							reader->Read(L"NonPlayerLvModifierInfoTable", nonPlayerLvModifierInfo);
						}
					}
					else
					{
						{
							Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateSerializerStructuredBinaryStreamReader(L"Database/NPCStatusModifierInfos.b");
							reader->Read(L"NonPlayerModifierInfoTable", nonPlayerModifierInfos);
						}

						{
							Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateSerializerStructuredBinaryStreamReader(L"Database/NPCLvModifierInfos.b");
							reader->Read(L"NonPlayerLvModifierInfoTable", nonPlayerLvModifierInfo);
						}
					}
				}
				void NonPlayerModifierTable::Save(bool xml)
				{
					if (xml == true)
					{
						{
							Resource::SerializerStreamWriter writer = Resource::ResourceSystemInstance().CreateSerializerXmlStreamWriter(L"Database/NPC/ModifierInfo/NPCStatusModifierInfos.xml");
							writer->Write(L"NonPlayerModifierInfoTable", nonPlayerModifierInfos);
						}

						{
							Resource::SerializerStreamWriter writer = Resource::ResourceSystemInstance().CreateSerializerXmlStreamWriter(L"Database/NPC/ModifierInfo/NPCLvModifierInfos.xml");
							writer->Write(L"NonPlayerLvModifierInfoTable", nonPlayerLvModifierInfo);
						}
					}
					else
					{
						{
							Resource::SerializerStreamWriter writer = Resource::ResourceSystemInstance().CreateSerializerStructuredBinaryStreamWriter(L"Database/NPCStatusModifierInfos.b");
							writer->Write(L"NonPlayerModifierInfoTable", nonPlayerModifierInfos);
						}

						{
							Resource::SerializerStreamWriter writer = Resource::ResourceSystemInstance().CreateSerializerStructuredBinaryStreamWriter(L"Database/NPCLvModifierInfos.b");
							writer->Write(L"NonPlayerLvModifierInfoTable", nonPlayerLvModifierInfo);
						}
					}
				}
			}
		}
	}
}
