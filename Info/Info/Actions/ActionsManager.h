#pragma once
#include <Info/Info/Loader.h>
#include "Actions.h"

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				class ActionInfoManager : public Serializer::ISerializable {
				public:
					struct Actions : public Serializer::ISerializable {
						typedef std::map< std::wstring/*unique name(not templateName)*/, std::vector<std::wstring>, Less<std::wstring> > ActorList;
						typedef std::map<std::wstring, ActionInfo, Less<std::wstring> > ActionList;

						ActionList actions;
						ActorList actorList;

						struct iterator {
							ActionList::iterator current;

							inline iterator(ActionList::iterator iter) { current = iter; }
							inline ActionInfo* operator* () { return &current->second; }
							inline void operator++ () { ++current; }
							inline bool operator== (const iterator& iter) { return (current == iter.current); }
							inline bool operator!= (const iterator& iter) { return (current != iter.current); }
						};

						inline iterator begin() { return iterator(actions.begin()); }
						inline iterator end() { return iterator(actions.end()); }

						inline ActionInfo* operator[] (const wchar_t* actionName) {
							if (actions.find(actionName) == actions.end())
								return NULL;
							return &actions[actionName];
						}
						inline ActionInfo* operator[] (uint32 actionName) {
							ActionList::iterator i = actions.begin();
							while (i != actions.end()) {
								uint32 hash = StringUtil::Hash((*i).first.c_str());
								if (hash == actionName)
									return &(*i).second;
								++i;
							}
							return NULL;
						}

						~Actions() {}

					public:
						void Serialize(Serializer::IStreamWriter& out) const;
						void Deserialize(Serializer::IStreamReader& in);
					};

					struct AutoAction : public Serializer::ISerializable {
						float startTime;
						Constants::Command command;
						uint32 param;
					public:
						void Serialize(Serializer::IStreamWriter& out) const;
						void Deserialize(Serializer::IStreamReader& in);
					};

				public:
					typedef std::map<std::wstring, Actions, Less<std::wstring> > ActionMap;
					typedef std::vector<AutoAction> AutoActionList;
					typedef std::map< uint32 /*unique id*/, std::vector<AutoAction> > AutoActionListMap;

					ActionMap actionMap;
					AutoActionListMap AutoActions;

					inline void Clear() { actionMap.clear(); }
					void LoadActionList(Actions::ActorList&, const wchar_t* xml/*, const wchar_t* name*/);
					void LoadActions(Actions::ActionList&, const wchar_t* xml);
					inline void LoadActionList(const std::wstring& templateName, const wchar_t* xml/*, const wchar_t* name*/) { LoadActionList(actionMap[templateName].actorList, xml); }
					inline void LoadActions(const std::wstring& templateName, const wchar_t* xml) { LoadActions(actionMap[templateName].actions, xml); }


				public:
					ActionInfoManager();
					virtual ~ActionInfoManager();

					void Init(bool bForceXml);
					void LoadXmlData();
					virtual void LoadBinaryData();
					void MakeLinkedActionLink();
					void MakeLinkedActionLinkToPart(const wchar_t* key, Actions& actions);

					virtual ActionInfo* Retrieve(const wchar_t* templateName, const wchar_t* actionName);
					virtual ActionInfo* Retrieve(uint32 templateName, uint32 actionName);
					virtual ActionInfo* Retrieve(XRated::Constants::ClassType classType, const wchar_t* actionName);
					virtual ActionInfo* Retrieve(XRated::Constants::ClassType classType, uint32 actionName);
					virtual const std::vector<std::wstring>* RetrieveActionList(const wchar_t* name, const wchar_t* templateName);
					virtual Actions& Retrieve(const wchar_t* templateName);
					void RetrieveTemplateName(std::vector<std::wstring>& l);

					const AutoActionList* RetriveAutoAction(uint32 hash) const;

				public:
					void Serialize(Serializer::IStreamWriter& out) const;
					void Deserialize(Serializer::IStreamReader& in);
				};
			}
		}
	}
}
