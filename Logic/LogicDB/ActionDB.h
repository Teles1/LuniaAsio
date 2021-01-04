#pragma once
#include "../Action/Action.h"
#include "StateDB.h"

namespace Lunia { namespace XRated { namespace Logic {

	class ActionDB {
		Database::Info::CompressedActionInfoManager &info;

		class ActionTemplate {
		public :
			struct ObjectActions {
				uint32 hashCode;
				std::wstring objectName;
				Action::Action::ActionPtrList actions;
			};
		private :
			Action::Action::ActionList allActions;
			std::vector<ObjectActions> objects;

		public :
			ActionTemplate();
			~ActionTemplate();

			void AddAction(Database::Info::ActionInfo* actionInfo);
			void AddActor(const std::wstring& objectName, std::vector<std::wstring>& actionList);
			Action::Action::ActionItem* GetAction(uint32 hashCode);
			Action::Action::ActionItem* GetAction(const std::wstring& name);
			Action::Action::ActionPtrList* GetActionList(uint32 hashCode);

		public:
			ActionTemplate(const ActionTemplate&); // this should not be used because of delete operation on destructor
		};

		std::map<uint32 /*template name hashCode*/, ActionTemplate> actionMap;

	private :
		void CreateActionTemplate(const std::wstring& templateName);

	public :
		ActionDB();
		~ActionDB();
		ActionDB& operator= (const ActionDB& db);

		void Init();

		//Retrieve actor's action list.
		Action::Action::ActionPtrList* GetActionList(uint32 templateCode, uint32 objCode);
		//For single debugging
		Action::Action::ActionPtrList* GetActionList(const std::wstring& templateName, const std::wstring obj);
	};

}	}	}
