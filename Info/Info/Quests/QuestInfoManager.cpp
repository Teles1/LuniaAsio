#include "QuestInfoManager.h"
#include <Core/FileIO/FileIO.h>
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				void QuestInfoManager::Init(bool bForceXmlData)
				{
					if (bForceXmlData) //Xml loading
						LoadXmlData();
					else //Binary loading
						LoadBinaryData();
				}

				void QuestInfoManager::LoadXmlData()
				{
					std::vector<std::wstring> filelist;
					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateDefaultDeserializer(L"Database/Quest/QuestList.xml");
					reader->Read(L"Quests", filelist);

					std::vector<std::wstring>::iterator iter;
					for (iter = filelist.begin(); iter != filelist.end(); ++iter)
						LoadQuestInfo((*iter).c_str());

					PostLoadProcess();
				}

				void QuestInfoManager::LoadBinaryData()
				{
					Resource::SerializerStreamReader reader =  Resource::ResourceSystemInstance().CreateDefaultDeserializer(L"Database/QuestInfos.b");
					reader->Read(L"QuestInfoManager", *this);
					PostLoadProcess();
				}

				void QuestInfoManager::LoadQuestInfo(const wchar_t* xml)
				{
					QUESTMap temp;
					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateDefaultDeserializer(xml);
					reader->Read(L"quests", temp);

					quests.insert(temp.begin(), temp.end());
				}

				void QuestInfoManager::Remove(uint32 id)
				{
					QUESTMap::iterator iter = quests.find(id);
					if (iter != quests.end())
					{
						quests.erase(iter++);
					}
					else
					{
						LoggerInstance().Exception("QuestInfoManager::Remove() - quest id {} not found. illegal remove procedure.", id);
					}
				}

				void QuestInfoManager::Add(const QUESTMap& addQuests)
				{
					if (addQuests.empty())return;

					QUESTMap::const_iterator addIter = addQuests.begin();
					while (addIter != addQuests.end())
					{
						QUESTMap::iterator iter = quests.find(addIter->first);
						if (iter != quests.end())
						{
							quests.erase(iter);
						}

						++addIter;
					}

					quests.insert(addQuests.begin(), addQuests.end());
				}

				void QuestInfoManager::Add(const std::map<uint32, QuestInfo>& addQuests)
				{
					if (addQuests.empty())return;

					auto addIter = addQuests.begin();
					while (addIter != addQuests.end())
					{
						quests[addIter->first] = addIter->second;
						for (QUESTMap::const_iterator ac = quests.begin(); ac != quests.end(); ++ac)
						{
							{
								for (std::vector<QuestInfo::Condition::Item>::const_iterator acitem = ac->second.AcceptCondition.Items.begin(); acitem != ac->second.AcceptCondition.Items.end(); ++acitem)
								{
									std::vector<uint32>::iterator iter = std::find(acItems.begin(), acItems.end(), acitem->ItemHash);
									if (iter == acItems.end()) acItems.push_back(acitem->ItemHash);
								}
							}

							{
								for (std::vector<QuestInfo::Condition::Item>::const_iterator ccitem = ac->second.CompleteCondition.Items.begin(); ccitem != ac->second.CompleteCondition.Items.end(); ++ccitem)
								{
									std::vector<uint32>::iterator iter = std::find(acItems.begin(), acItems.end(), ccitem->ItemHash);
									if (iter == acItems.end()) acItems.push_back(ccitem->ItemHash);
								}
							}
						}
						//QUESTMap::iterator iter = quests.find( addIter->first );
						//if ( iter != quests.end() )
						//{
						//	iter->second = addIter->second;
						//}
						//else
						//	quests.insert(*addIter);

						++addIter;
					}
				}


				void QuestInfoManager::AddPreiodQuests(const std::vector<uint32>& addPeriodQuests)
				{
					if (addPeriodQuests.empty())return;

					std::vector<uint32>::const_iterator addIter = addPeriodQuests.begin();
					while (addIter != addPeriodQuests.end())
					{
						std::vector<uint32>::iterator iter = std::find(periodQuests.begin(), periodQuests.end(), *addIter);
						if (iter == periodQuests.end())
						{
							periodQuests.push_back(*addIter);
						}

						++addIter;
					}
				}

				bool QuestInfoManager::IsExistQuest(uint32 questHash)
				{
					QUESTMap::const_iterator i = quests.find(questHash);
					return i != quests.end();
				}

				bool QuestInfoManager::IsExistACItemList(uint32 itemHash)
				{
					std::vector<uint32>::iterator iter = std::find(acItems.begin(), acItems.end(), itemHash);
					return iter != acItems.end();
				}

				const QuestInfo* QuestInfoManager::Retrieve(uint32 id) const
				{
					QUESTMap::const_iterator i = quests.find(id);
					if (i != quests.end())
						return &(i->second);
					return NULL;
				}

				void QuestInfoManager::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::QuestInfoManager");
					out.Write(L"quests", quests);
				}

				void QuestInfoManager::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::QuestInfoManager");
					in.Read(L"quests", quests);
				}

				void QuestInfoManager::PostLoadProcess()
				{
					periodQuests.clear();

					for (QUESTMap::const_iterator i = quests.begin(); i != quests.end(); ++i)
					{
						/* build stage reference */
						stageReference[i->second.AcceptLocation].push_back(std::pair<uint32, const QuestInfo*>(i->first, &(i->second)));

						/* find next quest */
						for (std::vector<uint32>::const_iterator q = i->second.AcceptCondition.Quests.begin(); q != i->second.AcceptCondition.Quests.end(); ++q)
						{
							QUESTMap::iterator r = quests.find(*q);
							if (r == quests.end())
								LoggerInstance().Exception(L"unable to find quest({}) in aceept condition of quest({})", *q, i->first);
							r->second.Next = std::pair<uint32, const QuestInfo*>(i->first, &(i->second));
						}

						if (i->second.IsHaveResetDate()) periodQuests.push_back(i->first);
					}

					epicQuests.clear();

					for (QUESTMap::const_iterator quest = quests.begin(); quest != quests.end(); ++quest)
					{
						if (0 == quest->second.AcceptLocation.StageGroupHash)
						{
							epicQuests[quest->second.AcceptCondition.Level].push_back(quest->first);
						}
					}

					acItems.clear();

					for (QUESTMap::const_iterator ac = quests.begin(); ac != quests.end(); ++ac)
					{
						{
							for (std::vector<QuestInfo::Condition::Item>::const_iterator acitem = ac->second.AcceptCondition.Items.begin(); acitem != ac->second.AcceptCondition.Items.end(); ++acitem)
							{
								std::vector<uint32>::iterator iter = std::find(acItems.begin(), acItems.end(), acitem->ItemHash);
								if (iter == acItems.end()) acItems.push_back(acitem->ItemHash);
							}
						}

						{
							for (std::vector<QuestInfo::Condition::Item>::const_iterator ccitem = ac->second.CompleteCondition.Items.begin(); ccitem != ac->second.CompleteCondition.Items.end(); ++ccitem)
							{
								std::vector<uint32>::iterator iter = std::find(acItems.begin(), acItems.end(), ccitem->ItemHash);
								if (iter == acItems.end()) acItems.push_back(ccitem->ItemHash);
							}
						}
					}
				}

				const std::vector< std::pair<uint32/*QuestHash*/, const QuestInfo*> >& QuestInfoManager::Retrieve(const StageLocation& stage) const
				{
					StageReferenceContainer::const_iterator i = stageReference.find(stage);
					return i->second;
				}


				const std::vector<uint32> QuestInfoManager::GetQuestList()
				{
					QUESTMap::const_iterator iter = quests.begin();
					QUESTMap::const_iterator end = quests.end();

					std::vector<uint32> list;
					while (iter != end)
					{
						list.push_back(iter->first);
						++iter;
					}
					return list;
				}

				const std::vector< uint32 > QuestInfoManager::GetEpicQuestList(const uint32 playerLevel) const
				{
					EpicQuests::const_iterator find = epicQuests.find(playerLevel);

					if (find != epicQuests.end())
					{
						return find->second;
					}
					return std::vector< uint32 >();
				}
				///////////////////////////////////////////////////////////////////// QuestInfoManager //

				// EventQuestInfoManager /////////////////////////////////////////////////////////////////////
				void EventQuestInfoManager::Init()
				{
					LoadXmlData();
				}

				void EventQuestInfoManager::LoadXmlData()
				{
					if (FileIO::File::Exists(L"EventQuestInfos.xml"))
					{
						Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateDefaultDeserializer(L"EventQuestInfos.xml");
						reader->Read(L"quests", quests);
						reader->Read(L"QuestObjectList", questObjectList);
						reader->Read(L"Quests", clientQuests);

						PostLoadProcess();
					}
				}

				void EventQuestInfoManager::PostLoadProcess()
				{
					periodQuests.clear();

					for (QUESTMap::iterator i = quests.begin(); i != quests.end(); ++i)
					{
						/* find next quest */
						for (std::vector<uint32>::const_iterator q = i->second.AcceptCondition.Quests.begin(); q != i->second.AcceptCondition.Quests.end(); ++q)
						{
							QUESTMap::iterator r = quests.find(*q);
							if (r == quests.end())
								throw ("unable to find quest({}) in aceept condition of quest(%d)", *q, i->first);
							r->second.Next = std::pair<uint32, const QuestInfo*>(i->first, &(i->second));
						}

						if (i->second.IsHaveResetDate()) periodQuests.push_back(i->first);
					}
				}

				void EventQuestInfoManager::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::QuestInfoManager");
					out.Write(L"quests", quests);
					out.Write(L"QuestObjectList", questObjectList);
					out.Write(L"Quests", clientQuests);
				}

				void EventQuestInfoManager::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::QuestInfoManager");
					in.Read(L"quests", quests);
					in.Read(L"QuestObjectList", questObjectList);
					in.Read(L"Quests", clientQuests);
				}

			}
		}
	}
}