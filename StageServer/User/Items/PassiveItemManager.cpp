#include "PassiveItemManager.h"

namespace Lunia { namespace XRated { namespace StageServer {

	void PassiveItemManager::Register(std::shared_ptr<NonLogicItem> item )
	{
		registered[item->GetTypeName()]=item;
	}

	/*
		passive item cannot be activated twice or more in a category. 
		caution ; passive item uses ItemInfo::SellCategory and ItemInfo::Price to avoid duplication
	*/
	void PassiveItemManager::Use(const std::vector<const Database::Info::ItemInfo*>& items)
	{
        /* filterating items by category */
		std::map<uint32/*category*/, const Database::Info::ItemInfo*> filteredItems;
		for(std::vector<const Database::Info::ItemInfo*>::const_iterator i=items.begin(); i!=items.end(); ++i)
		{
			const Database::Info::ItemInfo& item=*(*i);

			/* update filtered item */
			std::map<uint32, const Database::Info::ItemInfo*>::const_iterator i2=filteredItems.find( item.SellCategory );
			if (i2==filteredItems.end() || i2->second->SellPrice < item.SellPrice)
				filteredItems[item.SellCategory]=&item;
		}

		/* use passive item */
		activated.clear();
		for(std::map<uint32, const Database::Info::ItemInfo*>::const_iterator i=filteredItems.begin(); i!=filteredItems.end(); ++i)
		{
			auto i2	= registered.find( NonLogicItem::GetNoLogicTypeString( i->second ) );
			if (i2==registered.end())
			{
				LoggerInstance().Error(L"unregistered passive item (hash:{0})", i->second->Hash);
				continue;
			}

			activated.push_back(i->second);
			i2->second->Use( i->second, std::vector<String>() );
		}

	}

	const std::vector<const Database::Info::ItemInfo*>& PassiveItemManager::GetActivatedItems() const
	{
		return activated;
	}


} } }
