#include "ItemEx.h"
namespace Lunia {
	namespace XRated {
		namespace StageServer {

			void UseExItemManager::Register(NonLogicItemSharedPtr item)
			{
				noLogicItems[item->GetTypeName()] = item;
			}

			std::shared_ptr<NonLogicItem> UseExItemManager::Find(const Database::Info::ItemInfo* info)
			{
				Database::Info::ActionInfo::StateList::const_iterator i
					= std::find_if(info->States.begin(), info->States.end(), Database::Info::ActionInfo::StateFinder<Database::Info::StateInfo::NOLOGIC>());

				if (i == info->States.end()) {
					throw Exception(L"item ({0}:{1}) must have 'NoLogic State' parameter to UseItemEx", info->Hash, info->Id.c_str());
				}

				std::map<String, String>::const_iterator keyValue = i->params.find(L"Type");
				if (keyValue == i->params.end())
				{
					LoggerInstance().Error(L"item ({0}:{1}) must have 'Type' parameter to UseItemEx", info->Hash, info->Id.c_str());
					return NULL;
				}

				auto item = noLogicItems.find(keyValue->second);
				if (item == noLogicItems.end())
				{
					LoggerInstance().Error(L"item ({0}:{1}) is not registered for UseItemEx", info->Hash, info->Id.c_str());
					return NULL;
				}

				return item->second;
			}


		}
	}
}
