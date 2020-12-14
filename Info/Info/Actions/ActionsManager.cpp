#pragma once
#include "ActionsManager.h"
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				ActionInfoManager::ActionInfoManager()
				{
				}

				ActionInfoManager::~ActionInfoManager()
				{
					Clear();
				}

				void ActionInfoManager::Init(bool bForceXmlData)
				{
					Clear();

					if (bForceXmlData) {//Xml loading
						LoadXmlData();
					}
					else { //Binary loading
						std::wstring fullLoading;
						ActionInfoManager::LoadBinaryData();
					}

					MakeLinkedActionLink();
				}

				void ActionInfoManager::MakeLinkedActionLink()
				{
					ActionMap::iterator i = actionMap.begin();
					while (i != actionMap.end()) {
						Actions& actionTemplate = (*i).second;
						Actions::ActionList::iterator iAction = actionTemplate.actions.begin();
						while (iAction != actionTemplate.actions.end()) {
							{
								ActionInfo::ActionList& linkedActions = iAction->second.linkedActions;

								ActionInfo::ActionList::iterator iLinkedAction = linkedActions.begin();
								while (iLinkedAction != linkedActions.end()) {
									(*iLinkedAction).info = Retrieve((*i).first.c_str(), (*iLinkedAction).actionName.c_str());
									++iLinkedAction;
								}
							}
							{
								ActionInfo::ActionList& linkedActions = iAction->second.linkedActionsWhenHit;
								ActionInfo::ActionList::iterator iLinkedAction = linkedActions.begin();
								while (iLinkedAction != linkedActions.end()) {
									(*iLinkedAction).info = Retrieve((*i).first.c_str(), (*iLinkedAction).actionName.c_str());
									++iLinkedAction;
								}
							}

							++iAction;
						}
						++i;
					}
				}
				void ActionInfoManager::MakeLinkedActionLinkToPart(const wchar_t* key, Actions& actions)
				{
					Actions::ActionList::iterator iAction = actions.actions.begin();
					while (iAction != actions.actions.end()) {
						{
							ActionInfo::ActionList& linkedActions = iAction->second.linkedActions;

							ActionInfo::ActionList::iterator iLinkedAction = linkedActions.begin();
							while (iLinkedAction != linkedActions.end()) {
								(*iLinkedAction).info = Retrieve(key, (*iLinkedAction).actionName.c_str());
								++iLinkedAction;
							}
						}
						{
							ActionInfo::ActionList& linkedActions = iAction->second.linkedActionsWhenHit;
							ActionInfo::ActionList::iterator iLinkedAction = linkedActions.begin();
							while (iLinkedAction != linkedActions.end()) {
								(*iLinkedAction).info = Retrieve(key, (*iLinkedAction).actionName.c_str());
								++iLinkedAction;
							}
						}

						++iAction;
					}
				}

				void ActionInfoManager::LoadXmlData()
				{
					std::map<std::wstring, std::wstring> actionsXml;
					std::map<std::wstring, std::wstring> actionListXml;

					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateSerializerXmlStreamReader(L"Database/Actions/ActionList.xml");
					reader->Read(L"Actions", actionsXml);
					reader->Read(L"ActionLists", actionListXml);

					std::map<std::wstring, std::wstring>::const_iterator i = actionsXml.begin();


					for (; i != actionsXml.end(); ++i/*, ++zzz*/) {
						Actions& newAction = actionMap[i->first];
						LoadActions(newAction.actions, i->second.c_str());

						std::map<std::wstring, std::wstring>::const_iterator j = actionListXml.find(i->first);
						if (j == actionListXml.end())
							throw Exception(L"ActionList[{0}] is not Exist!", i->first.c_str());

						LoadActionList(newAction.actorList, j->second.c_str());
					}

					//Auto Action list
					{
						Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateSerializerXmlStreamReader(L"Database/Actions/AutoActionList.xml");
						std::vector<std::wstring> actionsXml;
						reader->Read(L"AutoActions", actionsXml);

						for (std::vector<std::wstring>::const_iterator i = actionsXml.begin(); i != actionsXml.end(); ++i) {
							AutoActionListMap temp;

							Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateDefaultDeserializer((*i).c_str());
							reader->Read(L"AutoActions", temp);

							AutoActions.insert(temp.begin(), temp.end());
						}
					}
				}

				void ActionInfoManager::LoadBinaryData()
				{
					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateDefaultDeserializer(L"Database/ActionInfos.b");
					reader->Read(L"ActionInfoManager", *this);
				}

				void ActionInfoManager::LoadActionList(Actions::ActorList& actorList, const wchar_t* xml/*, const wchar_t* name*/)
				{
					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateDefaultDeserializer(xml);
					reader->Read(L"Actions", actorList);

				}

				void ActionInfoManager::LoadActions(Actions::ActionList& actionList, const wchar_t* xml)
				{
					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateDefaultDeserializer(xml);
					reader->Read(L"Actions", actionList);
				}

				ActionInfo* ActionInfoManager::Retrieve(const wchar_t* templateName, const wchar_t* actionName)
				{
					if (actionMap.find(templateName) == actionMap.end())
						return NULL;
					return actionMap[templateName][actionName];
				}

				ActionInfo* ActionInfoManager::Retrieve(uint32 templateName, uint32 actionName)
				{
					ActionMap::iterator i = actionMap.begin();
					while (i != actionMap.end()) {
						uint32 hash = StringUtil::Hash((*i).first.c_str());
						if (hash == templateName)
							return (*i).second[actionName];
						++i;
					}
					return NULL;
				}

				ActionInfo* ActionInfoManager::Retrieve(XRated::Constants::ClassType classType, const wchar_t* actionName)
				{
					return Retrieve(Constants::GetClassStr(classType), actionName);
				}

				ActionInfo* ActionInfoManager::Retrieve(XRated::Constants::ClassType classType, uint32 actionName)
				{
					return Retrieve(Constants::GetClassHash(classType), actionName);
				}

				const std::vector<std::wstring>* ActionInfoManager::RetrieveActionList(const wchar_t* name, const wchar_t* templateName)
				{
					ActionMap::iterator iter = actionMap.find(templateName);
					if (iter == actionMap.end())
						return NULL;
					Actions& actions = (*iter).second;
					Actions::ActorList::iterator i = actions.actorList.find(name);
					if (i != actions.actorList.end())
						return &(*i).second;
					else
						return NULL;
				}

				ActionInfoManager::Actions& ActionInfoManager::Retrieve(const wchar_t* templateName)
				{
					ActionMap::iterator i = actionMap.find(templateName);
					if (i == actionMap.end())
					{
						throw Exception(L"unable to find action by template name({0})", templateName);
					}
					return (*i).second;
				}

				void ActionInfoManager::RetrieveTemplateName(std::vector<std::wstring>& l)
				{
					ActionMap::iterator i = actionMap.begin();
					for (; i != actionMap.end(); ++i)
						l.push_back((*i).first);
				}

				void ActionInfoManager::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"XRated::Database::Info::ActionInfoManager");
					out.Write(L"actionMap", actionMap);
					out.Write(L"AutoActions", AutoActions);
				}

				void ActionInfoManager::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"XRated::Database::Info::ActionInfoManager");
					in.Read(L"actionMap", actionMap);
					in.Read(L"AutoActions", AutoActions, AutoActionListMap());
				}

				void ActionInfoManager::Actions::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"XRated::Database::Info::ActionInfoManager::Actions");
					out.Write(L"actions", actions);
					out.Write(L"actorList", actorList);
				}

				void ActionInfoManager::Actions::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"XRated::Database::Info::ActionInfoManager::Actions");
					in.Read(L"actions", actions);
					in.Read(L"actorList", actorList);
				}

				//AutoAction
				const ActionInfoManager::AutoActionList* ActionInfoManager::RetriveAutoAction(uint32 hash) const
				{
					ActionInfoManager::AutoActionListMap::const_iterator iter = AutoActions.find(hash);
					if (iter != AutoActions.end()) {
						return &(*iter).second;
					}
					return NULL;
				}

				void ActionInfoManager::AutoAction::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"XRated::Database::Info::ActionInfoManager::AutoAction");
					out.Write(L"startTime", startTime);
					out.WriteEnum(L"command", command);
					out.Write(L"param", param);
				}

				void ActionInfoManager::AutoAction::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"XRated::Database::Info::ActionInfoManager::AutoAction");
					in.Read(L"startTime", startTime);
					in.ReadEnum(L"command", command);
					in.Read(L"param", param);
				}
			}
		}
	}
}