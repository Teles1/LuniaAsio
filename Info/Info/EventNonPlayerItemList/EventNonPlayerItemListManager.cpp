#pragma once
#include "EventNonPlayerItemListManager.h"
#include <Core/FileIO/File.h>
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				void EventNonPlayerItemList::NpcDropEvent::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"XRated::Database::Info::EventNonPlayerItemList::NpcDropEvent");
					out.Write(L"EventEnabled", EventEnabled);
					out.Write(L"EventItems", EventItems);
				}

				void EventNonPlayerItemList::NpcDropEvent::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"XRated::Database::Info::EventNonPlayerItemList::NpcDropEvent");
					in.Read(L"EventEnabled", EventEnabled, false);
					in.Read(L"EventItems", EventItems);
				}

				void EventNonPlayerItemList::Init()
				{
					Items.clear();

					LoadXmlData();
				}

				void EventNonPlayerItemList::LoadXmlData()
				{
					if (Lunia::FileIO::File::Exists(L"Event_NonPlayerDropItemTable.xml"))
					{
						////Load
						Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateDefaultDeserializer(L"Event_NonPlayerDropItemTable.xml");
						reader->Read(L"Items", Items);
					}
				}

				const NonPlayerInfo::Item* EventNonPlayerItemList::GetEventItem(float prob, uint32 nonPlayerHash, const XRated::StageLocation& stageLicense)
				{
					if (Items.empty()) return NULL;
					if (prob > 0.1f) return NULL;

					std::map<uint32, NpcDropEvent>::const_iterator iter = Items.begin();
					while (iter != Items.end())
					{
						if (iter->second.EventEnabled)
						{
							float itemProb = 0.f;
							std::map<uint32/*NpcHash*/, std::vector<NonPlayerInfo::Item> >::const_iterator npcIter = iter->second.EventItems.NpcItems.find(nonPlayerHash);
							if (npcIter != iter->second.EventItems.NpcItems.end())
							{
								std::vector<NonPlayerInfo::Item>::const_iterator itemIter = npcIter->second.begin();
								while (itemIter != npcIter->second.end())
								{
									if (itemIter->Probability != 0.0f)
									{
										itemProb += itemIter->Probability;
										if (prob <= itemProb)
										{
											return &(*itemIter);
										}
									}

									++itemIter;
								}
							}

							itemProb = 0.f;
							std::map<XRated::StageLocation, std::vector<NonPlayerInfo::Item> >::const_iterator stageIter = iter->second.EventItems.StageItems.find(stageLicense);
							if (stageIter != iter->second.EventItems.StageItems.end())
							{
								std::vector<NonPlayerInfo::Item>::const_iterator itemIter = stageIter->second.begin();
								while (itemIter != stageIter->second.end())
								{
									if (itemIter->Probability != 0.0f)
									{
										itemProb += itemIter->Probability;
										if (prob <= itemProb)
										{
											return &(*itemIter);
										}
									}

									++itemIter;
								}
							}

							itemProb = 0.f;
							std::vector<NonPlayerInfo::Item>::const_iterator anyWhereItemIter = iter->second.EventItems.AnyWhereItems.begin();
							while (anyWhereItemIter != iter->second.EventItems.AnyWhereItems.end())
							{
								if (anyWhereItemIter->Probability != 0.0f)
								{
									itemProb += anyWhereItemIter->Probability;
									if (prob <= itemProb)
									{
										return &(*anyWhereItemIter);
									}
								}

								++anyWhereItemIter;
							}
						}

						++iter;
					}

					return NULL;
				}

				void EventNonPlayerItemList::EnableEvent(uint32 eventID, bool enable)
				{
					if (Items.empty()) return;

					std::map<uint32, NpcDropEvent>::iterator iter = Items.find(eventID);
					if (iter != Items.end())
					{
						iter->second.EventEnabled = enable;
					}
				}

				void EventNonPlayerItemList::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"XRated::Database::Info::EventNonPlayerItemList");
					out.Write(L"Items", Items);
				}

				void EventNonPlayerItemList::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"XRated::Database::Info::EventNonPlayerItemList");
					in.Read(L"Items", Items);
				}
			}
		}
	}
}