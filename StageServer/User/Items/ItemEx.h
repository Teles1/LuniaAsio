#pragma once
#include <Info/Info.h>
#include "NoLogicItems.h"
namespace Lunia {
	namespace XRated {
		namespace StageServer {
			typedef std::shared_ptr<NonLogicItem> NonLogicItemSharedPtr;
			class UseExItemManager
			{
			public:
				NonLogicItemSharedPtr Find(const Database::Info::ItemInfo* info);
				void Register(NonLogicItemSharedPtr item);

			private:
				std::map<String, NonLogicItemSharedPtr> noLogicItems;
			};

		}
	}
}
