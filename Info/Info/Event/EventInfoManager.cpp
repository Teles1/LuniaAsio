#pragma once
#include "EventInfoManager.h"

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				void EventInfoManager::Load(bool xml)
				{
					if (xml)
					{
						Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateSerializerXmlStreamReader(L"Database/StageEventInfo.xml");
						reader->Read(L"EventList", infos);

						reader = Resource::ResourceSystemInstance().CreateSerializerXmlStreamReader(L"Database/ScriptEventNPCInfo.xml");
						reader->Read(L"NPCList", scriptEventNPCs);
					}
					else
					{
						Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateSerializerStructuredBinaryStreamReader(L"Database/StageEventInfo.b");
						reader->Read(L"EventList", infos);

						reader = Resource::ResourceSystemInstance().CreateSerializerStructuredBinaryStreamReader(L"Database/ScriptEventNPCInfo.b");
						reader->Read(L"NPCList", scriptEventNPCs);
					}
				}

				void EventInfoManager::Save(bool xml)
				{
					if (xml)
					{
						Resource::SerializerStreamWriter writer = Resource::ResourceSystemInstance().CreateSerializerXmlStreamWriter(L"Database/StageEventInfo.xml");
						writer->Write(L"EventList", infos);

						writer = Resource::ResourceSystemInstance().CreateSerializerXmlStreamWriter(L"Database/ScriptEventNPCInfo.xml");
						writer->Write(L"NPCList", scriptEventNPCs);
					}
					else
					{
						Resource::SerializerStreamWriter writer = Resource::ResourceSystemInstance().CreateSerializerStructuredBinaryStreamWriter(L"Database/StageEventInfo.b");
						writer->Write(L"EventList", infos);

						writer = Resource::ResourceSystemInstance().CreateSerializerStructuredBinaryStreamWriter(L"Database/ScriptEventNPCInfo.b");
						writer->Write(L"NPCList", scriptEventNPCs);
					}
				}

				void EventInfoManager::GetEventList(std::vector<EventUnit>& eventList)
				{
					eventList.clear();

					eventList = infos[-1];

					std::map< int /*nationalCode*/, std::vector<EventUnit> >::iterator findIter = infos.find(Constants::LocaleSpecificSetting::COUNTRY_CODE);
					std::map< int /*nationalCode*/, std::vector<EventUnit> >::iterator endIter = infos.end();

					if (findIter != endIter)
					{
						eventList.insert(eventList.end(), findIter->second.begin(), findIter->second.end());
					}
				}

				uint32 EventInfoManager::GetEventNPC(uint32 eventID, uint32 oldNPCHash)
				{
					std::map< int /*nationalCode*/, std::map< uint32 /*eventID*/, ChangeNPCTable > >::iterator findIter = scriptEventNPCs.find(Constants::LocaleSpecificSetting::COUNTRY_CODE);
					std::map< int /*nationalCode*/, std::map< uint32 /*eventID*/, ChangeNPCTable > >::iterator endIter = scriptEventNPCs.end();

					if (findIter != endIter)
					{
						std::map< uint32 /*eventID*/, ChangeNPCTable >::iterator findSubIter = findIter->second.find(eventID);
						std::map< uint32 /*eventID*/, ChangeNPCTable >::iterator endSubIter = findIter->second.end();

						if (findSubIter != endSubIter)
						{
							ChangeNPCTable::iterator findNPCIter = findSubIter->second.find(oldNPCHash);
							ChangeNPCTable::iterator endNPCIter = findSubIter->second.end();

							if (findNPCIter != endNPCIter)
							{
								return findNPCIter->second;
							}
						}
					}

					findIter = scriptEventNPCs.find(-1); // find Default

					if (findIter != endIter)
					{
						std::map< uint32 /*eventID*/, ChangeNPCTable >::iterator findSubIter = findIter->second.find(eventID);
						std::map< uint32 /*eventID*/, ChangeNPCTable >::iterator endSubIter = findIter->second.end();

						if (findSubIter != endSubIter)
						{
							ChangeNPCTable::iterator findNPCIter = findSubIter->second.find(oldNPCHash);
							ChangeNPCTable::iterator endNPCIter = findSubIter->second.end();

							if (findNPCIter != endNPCIter)
							{
								return findNPCIter->second;
							}
						}
					}

					return 0;
				}

			}
		}
	}
}
