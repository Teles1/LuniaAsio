#pragma once
#include "IndexedActionsInfo.h"

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {

				void ActionLoaderManager::Init(const String& fileName)
				{
					LoaderManager::Init(fileName);
				}
				template <typename T>
				void ActionLoaderManager::Init(const String& filename, T& data)
				{
					LoaderManager::Init(filename);

					fileReader->SetReadCursor(fileOfEnd, IStream::CursorPosition::Begin);

					Resource::SerializerStreamReader readStream = Serializer::CreateBinaryStreamReader(fileReader);

					uint32 size;
					readStream->Read(L"size", size);

					std::wstring readhash, readhash2;

					for (uint32 i = 0; i < size; ++i)
					{
						readStream->Read(L"header", readhash);
						dataPointer[StringUtil::Hash(readhash.c_str())].StartPos = fileReader->GetReadCursor();

						uint32 subsize;
						readStream->Read(L"size", subsize);

						for (uint32 j = 0; j < subsize; ++j)
						{
							readStream->Read(L"header", readhash2);

							//			uint32 hash = StringUtil::Hash(readhash2.c_str());
							data.actions[readhash2].Deserialize(*readStream);
						}
						// make hash to template name table(make by kim).
						uint32 templateHash = StringUtil::Hash(readhash.c_str());
						actionTemplateMap[templateHash] = readhash;

						readStream->Read(L"actorList", data.actorList);
						dataPointer[StringUtil::Hash(readhash.c_str())].EndPos = fileReader->GetReadCursor();
					}

					fileOfEnd = fileReader->GetReadCursor();

				}
				const wchar_t* ActionLoaderManager::GetTemplateName(uint32 key)
				{
					std::map<uint32, std::wstring>::iterator iter = actionTemplateMap.find(key);
					if (iter != actionTemplateMap.end()) {
						return iter->second.c_str();
					}
					return NULL;
				}

				void ActionLoaderManager::LoadTemplateTable(const std::wstring& file)
				{
					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateSerializerStructuredBinaryStreamReader(file.c_str());
					reader->Read(L"ActionLoaderManager::TemplateTable", actionTemplateMap);
				}
				void ActionLoaderManager::SaveTemplateTable(const std::wstring& file)
				{
					Resource::SerializerStreamWriter writer = Resource::ResourceSystemInstance().CreateSerializerStructuredBinaryStreamWriter(file.c_str());
					writer->Write(L"ActionLoaderManager::TemplateTable", actionTemplateMap);
				}
				void IndexedActionInfoManager::LoadBinaryData()
				{
					manager.Init(L"Database/ActionInfos.b");
					manager.Load(L"Database/ActionInfosIndex.b");
					manager.LoadTemplateTable(L"Database/ActionTemplateTable.b");
				}

				void IndexedActionInfoManager::MakeIndexAndSave()
				{
					XRated::Database::Info::ActionInfoManager::Actions data;
					manager.Init(L"Database/ActionInfos.b", data);
					manager.Save(L"Database/ActionInfosIndex.b");
					manager.SaveTemplateTable(L"Database/ActionTemplateTable.b");

				}

				ActionInfo* IndexedActionInfoManager::Retrieve(const wchar_t* templateName, const wchar_t* actionName)
				{
					if (actionMap.find(templateName) != actionMap.end())
						return actionMap[templateName][actionName];
					else
					{
						uint32 hash = StringUtil::Hash(templateName);
						XRated::Database::Info::ActionInfoManager::Actions data;
						if (manager.Retrieve(hash, data))
						{
							actionMap[templateName] = data;
							// make linked action link
							MakeLinkedActionLinkToPart(templateName, actionMap[templateName]);
							return actionMap[templateName][actionName];
						}
						else
							return NULL;
					}
				}

				ActionInfo* IndexedActionInfoManager::Retrieve(uint32 templateName, uint32 actionName)
				{
					ActionInfoManager::ActionMap::iterator i = actionMap.begin();
					while (i != actionMap.end()) {
						uint32 hash = StringUtil::Hash((*i).first.c_str());
						if (hash == templateName)
							return (*i).second[actionName];
						++i;
					}


					const wchar_t* templateNameString = manager.GetTemplateName(templateName);
					if (templateNameString != NULL) {
						XRated::Database::Info::ActionInfoManager::Actions data;
						if (manager.Retrieve(templateName, data))
						{
							actionMap[templateNameString] = data;
							MakeLinkedActionLinkToPart(templateNameString, actionMap[templateNameString]);
							return actionMap[templateNameString][actionName];
						}
					}
					else {
						//ALLM_ERROR((L"IndexedActionInfoManager : can not found action template name Hash : %d", templateName));
					}

					return NULL;
				}

				ActionInfo* IndexedActionInfoManager::Retrieve(XRated::Constants::ClassType classType, const wchar_t* actionName)
				{
					switch (classType)
					{
					case XRated::Constants::ClassType::Healer:
						return Retrieve(L"Eir", actionName);
					case XRated::Constants::ClassType::Knight:
						return Retrieve(L"Sieg", actionName);
					case XRated::Constants::ClassType::Wizard:
						return Retrieve(L"Dainn", actionName);
					case XRated::Constants::ClassType::Thief:
						return Retrieve(L"Tia", actionName);
					default:
						throw;
					}

					//	return NULL;
				}

				ActionInfo* IndexedActionInfoManager::Retrieve(XRated::Constants::ClassType classType, uint32 actionName)
				{
					return Retrieve(Constants::GetClassHash(classType), actionName);
				}

				const std::vector<std::wstring>* IndexedActionInfoManager::RetrieveActionList(const wchar_t* name, const wchar_t* templateName)
				{
					ActionInfoManager::ActionMap::iterator iter = actionMap.find(templateName);
					if (iter == actionMap.end())
					{
						uint32 hash = StringUtil::Hash(templateName);
						XRated::Database::Info::ActionInfoManager::Actions data;
						if (manager.Retrieve(hash, data))
						{
							actionMap[templateName] = data;
							MakeLinkedActionLinkToPart(templateName, actionMap[templateName]);
						}
					}
					Actions& actions = (*iter).second;
					Actions::ActorList::iterator i = actions.actorList.find(name);
					if (i != actions.actorList.end())
						return &(*i).second;
					else
						return NULL;
				}

				ActionInfoManager::Actions& IndexedActionInfoManager::Retrieve(const wchar_t* templateName)
				{
					if (actionMap.find(templateName) != actionMap.end())
						return actionMap[templateName];
					else
					{
						uint32 hash = StringUtil::Hash(templateName);
						XRated::Database::Info::ActionInfoManager::Actions data;
						if (manager.Retrieve(hash, data))
						{
							actionMap[templateName] = data;
							// make linked action link
							MakeLinkedActionLinkToPart(templateName, actionMap[templateName]);
							return actionMap[templateName];
						}
					}

					return actionMap[templateName];
				}

				bool IndexedActionInfoManager::Remove(uint32 hash)
				{
					ActionInfoManager::ActionMap::iterator i = actionMap.begin();
					while (i != actionMap.end()) {
						uint32 findhash = StringUtil::Hash((*i).first.c_str());
						if (findhash == hash)
						{
							actionMap.erase(i);
							return true;
						}
						++i;
					}

					return false;
				}

				bool IndexedActionInfoManager::Remove(const wchar_t* id)
				{
					ActionInfoManager::ActionMap::iterator it = actionMap.find(id);
					if (it != actionMap.end())
					{
						actionMap.erase(it);
						return true;
					}

					return false;
				}

			}
		}
	}
}