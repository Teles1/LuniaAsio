#pragma once
#include <map>
#include "NoLogicItems.h"

namespace Lunia { namespace XRated { namespace StageServer {
	class PassiveItemManager
	{
	public:
		void Register(std::shared_ptr<NonLogicItem> item );
		void Use(const std::vector<const Database::Info::ItemInfo*>& items);
		const std::vector<const Database::Info::ItemInfo*>& GetActivatedItems() const;
	private:
		std::map< std::wstring, std::shared_ptr<NonLogicItem> > registered;
		std::vector<const Database::Info::ItemInfo*> activated;
	};

} } }
