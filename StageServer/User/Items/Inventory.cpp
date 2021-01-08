#include "Inventory.h"
#include <algorithm>

namespace Lunia { namespace XRated { namespace StageServer {


	Inventory::Inventory(uint32 maxBagCount, uint32 slotCount, bool isBank) : MAXBAG(maxBagCount), SLOTCOUNT(slotCount), ISBANK(isBank)
	{
		//bags.reserve(MAXBAG);
		//for(size_t i = 0 ; i < MAXBAG ; ++i) 
		//	bags.push_back(Bag(i, slotCount, isBank?1:0)); // 0 == inventoryBag, 1 == bankBag;
	}

	Inventory::~Inventory()
	{
	}

	void Inventory::Clear()
	{
		for( std::map<int,Bag>::iterator it=bags.begin(), endIt=bags.end(); it!=endIt; ++it )
		{
			it->second.Clear();
		}
	}

	bool Inventory::Exist( uint32 bag, uint32 pos ) const
	{
		Bags::const_iterator iter = bags.find( bag );
		if ( iter != bags.end() && pos < SLOTCOUNT )
		{
			return true;
		}

		return false;
	}
	const Common::ItemEx* Inventory::GetItem(uint32 bag, uint32 pos) const
	{
		if( !Exist( bag, pos ) )
		{
			LoggerInstance().Error("Inventory::GetItem({0},{1})", bag, pos );
			return NULL;
		}

		Bags::const_iterator iter = bags.find( bag );

		const Common::ItemEx& item = iter->second.GetItem(pos);
		if (item.Info==NULL || item.StackedCount==0) return NULL;
		return &item;
	}

	const Common::ItemEx* Inventory::GetItem(const ItemPosition& pos) const
	{
        return GetItem(pos.Bag, pos.Position);
	}

	std::vector<const Common::ItemEx*> Inventory::GetItems(const Common::ItemEx::IFindCondition& condition, bool includeDisabled) const
	{
		std::vector< const Common::ItemEx* > ret;
		for ( Bags::const_iterator iter = bags.begin(); iter != bags.end(); ++iter )
		{
			const Bag& b = iter->second;
			if ( ( includeDisabled == false ) && ( b.IsEnabled() == false ) )
			{
				continue; // skip disabled bag
			}

			for ( size_t pos = 0; pos < b.GetBagSize(); ++pos )
			{
				const Common::ItemEx& item=b.GetItem( pos );
				if ( item.Info == NULL )
				{
					continue;
				}
				if ( condition( item ) )
				{
					ret.push_back( &item );
				}
			}
		}

		return ret;
	}

	std::vector< const Common::ItemEx* > Inventory::GetItemsInNormalBag( const Common::ItemEx::IFindCondition& condition, bool includeDisabled) const
	{
		std::vector<uint8> bagIndexis;
		Database::DatabaseInstance().InfoCollections.BagInfos.GetBagNumbers( Database::Info::BagInfos::BagType::Normal, bagIndexis );

		std::vector<const Common::ItemEx*> ret;
		for ( std::vector<uint8>::const_iterator iter = bagIndexis.begin(); iter != bagIndexis.end(); ++iter )
		{
			Bags::const_iterator iter2 = bags.find( *iter );
			if ( iter2 != bags.end() )
			{
				const Bag& b = iter2->second;
				if (includeDisabled==false && b.IsEnabled()==false) continue; // skip disabled bag

				for(size_t pos = 0; pos < b.GetBagSize(); ++pos)
				{
					const Common::ItemEx& item=b.GetItem(pos);
					if (item.Info==NULL) continue;
					if (condition(item)) ret.push_back(&item);
				}
			}
		}

		return ret;
	}

	uint32 Inventory::GetCount(const Common::ItemEx::ICountCondition& condition) const
	{
		uint32 count(0);
		for(Bags::const_iterator it=bags.begin(), endIt=bags.end(); it!=endIt; ++it)
		{
			if (it->second.IsEnabled()==false) continue; // skip disabled bag
			count += it->second.GetCount(condition);
		}
		return count;
	}

	uint32 Inventory::GetItemCount(uint32 itemHash) const
	{
		struct CountCondition : public Common::ItemEx::ICountCondition
		{
			uint32 hash;
			CountCondition(uint32 hash) : hash(hash) {}
			uint32 operator()(const Common::ItemEx& rhs) const { return (rhs.Info!=NULL && rhs.Info->Hash==hash)?rhs.StackedCount:0; }
		};
        
		return GetCount( CountCondition(itemHash) );
	}

	uint32 Inventory::GetEmptySlotCountByType( const Database::Info::ItemInfo* item ) const
	{
		struct CountCondition : public Common::ItemEx::ICountCondition
		{
			uint32 operator()(const Common::ItemEx& rhs) const { return (rhs.Info==NULL)?1:0; }
		};
        
		uint32 count(0);
		for(Bags::const_iterator it=bags.begin(), endIt=bags.end(); it!=endIt; ++it)
		{
			if (it->second.IsEnabled()==false) 
			{
				continue; // skip disabled bag
			}

			if( !IsEnableItem(it->first, item) )
			{
				continue;
			}

			count += it->second.GetCount(CountCondition());
		}
		return count;
	}
	uint32 Inventory::GetEmptySlotCount() const
	{
		return GetEmptySlotCountByType(NULL);
		//struct CountCondition : public Common::ItemEx::ICountCondition
		//{
		//	uint32 operator()(const Common::ItemEx& rhs) const { return (rhs.Info==NULL)?1:0; }
		//};
  //      
		//return GetCount( CountCondition());
	}

	uint32 Inventory::GetBlankSlotCountOfNormalBag() const
	{
		std::vector<uint8> bagIndexis;
		Database::DatabaseInstance().InfoCollections.BagInfos.GetBagNumbers( Database::Info::BagInfos::BagType::Normal, bagIndexis );

		struct CountCondition : public Common::ItemEx::ICountCondition
		{
			uint32 operator()(const Common::ItemEx& rhs) const { return (rhs.Info==NULL)?1:0; }
		};
        
		uint32 count(0);
		for ( std::vector<uint8>::const_iterator iter = bagIndexis.begin(); iter != bagIndexis.end(); ++iter )
		{
			Bags::const_iterator iter2 = bags.find( *iter );
			if ( iter2 != bags.end() )
			{				
				if ( iter2->second.IsEnabled()==false) 
				{
					continue; // skip disabled bag
				}

				if ( !IsEnableItem( *iter, NULL ) )
				{
					continue;
				}

				count += iter2->second.GetCount( CountCondition() );
			}
		}
		return count;
	}

	XRated::ItemPosition Inventory::FindItemPosition(const Common::ItemEx::IFindCondition& condition, bool checkDisabledBag) const
	{
		for(Bags::const_iterator it=bags.begin(), endIt=bags.end(); it!=endIt; ++it)
		{
			if( checkDisabledBag == false ) 
			{
				if (it->second.IsEnabled()==false) 
					continue; // skip disabled bag
			}

			int pos = it->second.FindItem(condition);
			if (pos != Bag::InvalidPosition )
				return ItemPosition(it->first, static_cast<uint8>(pos));
		}

		return ItemPosition::Invalid;
	}

	XRated::ItemPosition Inventory::FindEmptySlotByType(const Common::ItemEx::IFindCondition& condition, const Database::Info::ItemInfo* item, bool checkDisabledBag) const
	{
		for( int i=Database::Info::BagInfos::BagType::MaxCnt - 1; i >= Database::Info::BagInfos::BagType::Normal; --i )
		{
			std::vector<uint8> typeBags;
			Database::DatabaseInstance().InfoCollections.BagInfos.GetBagNumbers( static_cast<Database::Info::BagInfos::BagType>(i), typeBags );
			for( size_t index=0; index<typeBags.size(); ++index )
			{
				Bags::const_iterator it = bags.find( typeBags[index] );
				if( it != bags.end() )
				{
					if( it->second.IsEnabled()==false ) 
					{
						continue; // skip disabled bag
					}
					
					if( !IsEnableItem(it->first, item) )
					{
						continue;
					}
					
					int pos = it->second.FindItem(condition);
					if (pos != Bag::InvalidPosition )
						return ItemPosition(it->first, static_cast<uint8>(pos));
				}
			}
		}
		return ItemPosition::Invalid;
	}

	std::vector<XRated::ItemPosition> Inventory::FindEmptySlotsByType(const Common::ItemEx::IFindCondition& condition, const Database::Info::ItemInfo* item) const
	{
		std::vector<XRated::ItemPosition> positions;
	
		for( int i=Database::Info::BagInfos::BagType::MaxCnt - 1; i >= Database::Info::BagInfos::BagType::Normal; --i )
		{
			std::vector<uint8> typeBags;
			Database::DatabaseInstance().InfoCollections.BagInfos.GetBagNumbers( static_cast<Database::Info::BagInfos::BagType>(i), typeBags );
			for( size_t index=0; index<typeBags.size(); ++index )
			{
				Bags::const_iterator it = bags.find( typeBags[index] );
				if( it != bags.end() )
				{
					if( it->second.IsEnabled()==false ) 
					{
						continue; // skip disabled bag
					}
					
					if( !IsEnableItem(it->first, item) )
					{
						continue;
					}
					
					int pos(-1);
					while( (pos = it->second.FindItem(condition, pos+1)) != Bag::InvalidPosition )
					{
						positions.push_back( XRated::ItemPosition(it->first, static_cast<uint8>(pos) ) );
					}
				}
			}
		}

		return positions;
	}
    
	std::vector<XRated::ItemPosition> Inventory::FindItemPositions(const Common::ItemEx::IFindCondition& condition) const
	{
		std::vector<XRated::ItemPosition> positions;

		for(Bags::const_iterator it=bags.begin(), endIt=bags.end(); it!=endIt; ++it)
		{
			if (it->second.IsEnabled()==false) continue; // skip disabled bag
			int pos(-1);
			while( (pos = it->second.FindItem(condition, pos+1)) != Bag::InvalidPosition )
			{
				positions.push_back( XRated::ItemPosition(it->first, static_cast<uint8>(pos) ) );
			}
		}
		return positions;
	}


	XRated::ItemPosition Inventory::FindEmptySlot( const Database::Info::ItemInfo* item ) const
	{
		struct Condition : public Common::ItemEx::IFindCondition
		{
			bool operator()(const Common::ItemEx& rhs) const { if (rhs.Info==NULL) return true; return false; }
		};

		return FindEmptySlotByType( Condition(), item );
	}

	XRated::ItemPosition Inventory::FindItem(uint32 itemHash) const
	{
		return FindItemPosition(Common::ItemEx::FindCondition(itemHash));
	}

	ItemPosition Inventory::FindStackableSlot(const Database::Info::ItemInfo* item, InstanceEx instance, uint16 count) const
	{
		struct Condition : public Common::ItemEx::IFindCondition
		{
			uint32 hash; uint16 count; InstanceEx instance;
			Condition(uint32 hash, InstanceEx instance, uint16 count) : hash(hash), count(count), instance(instance) {}
			bool operator()(const Common::ItemEx& rhs) const
			{
				if (rhs.Info==NULL || rhs.Info->Hash!=hash || rhs.InstanceEx != instance)
					return false;

				if (rhs.Info->MaxCnt >= static_cast<int32>(count+rhs.StackedCount))
					return true;

				return false;
			}
		};

		return FindEmptySlotByType( Condition(item->Hash, instance, count), item );
	}

	std::vector<XRated::ItemPosition> Inventory::FindStackableSlots(const Database::Info::ItemInfo* item, InstanceEx instance, uint16 count, bool checkEmptySlot) const
	{
		struct Condition : public Common::ItemEx::IFindCondition
		{
			uint32 hash; InstanceEx instance;
			mutable uint16 count;
			Condition(uint32 hash, InstanceEx instance, uint16 count) : hash(hash), count(count), instance(instance) {}
			bool operator()(const Common::ItemEx& rhs) const
			{
				if(rhs.Info == NULL || rhs.Info->Hash!=hash || rhs.InstanceEx != instance)
					return false;

				if(count == 0 || rhs.Info->MaxCnt == rhs.StackedCount)
					return false;

				if(rhs.Info->MaxCnt >= static_cast<int32>(count + rhs.StackedCount))
					count = 0;
				else
					count = static_cast<int32>(count+rhs.StackedCount) - rhs.Info->MaxCnt;

				return true;
			}
		};
		
		Condition condition(item->Hash, instance, count);

		std::vector<XRated::ItemPosition> positions = FindEmptySlotsByType(condition, item);

		if(condition.count != 0)
		{
			if(checkEmptySlot == true)
			{
				XRated::ItemPosition pos = FindEmptySlot(item);

				if(pos != XRated::ItemPosition::Invalid)
					positions.push_back(pos);				
				else
					positions.clear();
			}
			else
				positions.clear();
		}

		return positions;
	}

	uint32 Inventory::CountRemainStackableItem(uint32 itemHash, InstanceEx instance, uint32 count) const
	{
		struct Condition : public Common::ItemEx::IFindCondition
		{
			uint32 hash; InstanceEx instance;
			mutable uint32 count;
			Condition(uint32 hash, InstanceEx instance, uint32 count) : hash(hash), count(count), instance(instance) {}
			bool operator()(const Common::ItemEx& rhs) const
			{
				if(rhs.Info == NULL || rhs.Info->Hash!=hash || rhs.InstanceEx != instance)
					return false;

				if(count == 0 || rhs.Info->MaxCnt == rhs.StackedCount)
					return false;

				if(rhs.Info->MaxCnt >= static_cast<int32>(count + rhs.StackedCount))
					count = 0;
				else
					count = static_cast<int32>(count+rhs.StackedCount) - rhs.Info->MaxCnt;

				return true;
			}
		};
		
		Condition condition(itemHash, instance, count);

		std::vector<XRated::ItemPosition> positions = FindItemPositions(condition);

		return condition.count;
	}

	uint32 Inventory::CountRemainAfterAddItem(uint32 itemHash, InstanceEx instance, uint32 count, uint32 maxCnt) const
	{
		struct Condition : public Common::ItemEx::IFindCondition
		{
			uint32 hash; InstanceEx instance;
			mutable uint32 count, maxCnt;
			Condition(uint32 hash, InstanceEx instance, uint32 count, uint32 maxCnt) : hash(hash), count(count), maxCnt(maxCnt), instance(instance) {}
			bool operator()(const Common::ItemEx& rhs) const
			{
				if(rhs.Info == NULL )
				{
					if( maxCnt >= count )
						count = 0;
					else
						count -= maxCnt;

					return true;
				}

				if(rhs.Info->Hash!=hash || rhs.InstanceEx != instance)
					return false;

				if(count == 0 || rhs.Info->MaxCnt == rhs.StackedCount)
					return false;

				if(rhs.Info->MaxCnt >= static_cast<int32>(count + rhs.StackedCount))
					count = 0;
				else
					count = static_cast<int32>(count+rhs.StackedCount) - rhs.Info->MaxCnt;

				return true;
			}
		};
		
		Condition condition(itemHash, instance, count, maxCnt);

		std::vector<XRated::ItemPosition> positions = FindItemPositions(condition);

		return condition.count;
	}

	XRated::ItemPosition Inventory::FindPetItem( XRated::GlobalSerial itemSerial )
	{
		for(Bags::const_iterator it=bags.begin(), endIt=bags.end(); it!=endIt; ++it)
		{
			if (it->second.IsEnabled()==false) continue; // skip disabled bag
			int pos = it->second.FindPetItem(itemSerial);
			if (pos != Bag::InvalidPosition )
				return ItemPosition(it->first, static_cast<uint8>(pos));
		}
		return XRated::ItemPosition::Invalid;
	}

	bool Inventory::SetItem(uint32 bag, uint32 pos, const Common::ItemEx& item)
	{
		if( !Exist(bag,pos) )
		{
			LoggerInstance().Error("Inventory::PushItems() - bag({0}) > bags.size()", bag );
			return false;
		}

		return bags[bag].SetItem(pos, item);
	}
	

	bool Inventory::AddItem(uint32 bag, uint32 pos, uint32 count)
	{
		if( !Exist(bag,pos) )
		{
			LoggerInstance().Error("Inventory::AddItems() - bag({0}) > bags.size()", bag );
			return false;
		}
		return bags[bag].AddItem(pos, static_cast<uint16>(count));
	}

	bool Inventory::DropItem(uint32 bag, uint32 pos, uint32 count, Common::ItemEx* droppedItem)
	{
		if( !Exist(bag,pos) )
		{
			LoggerInstance().Error("Inventory::DropItems() - bag({0}) > bags.size()", bag );
			return false;
		}

		return bags[bag].DropItem(pos, static_cast<uint16>(count), droppedItem);
	}

	void Inventory::DeleteItem(uint32 bag, uint32 pos)
	{
		if( !Exist(bag,pos) )
		{
			LoggerInstance().Error("Inventory::DeleteItems() - bag({0}) > bags.size()", bag );
			return;
		}
		bags[bag].DeleteItem(pos);
	}

	uint32 Inventory::DeleteItems(uint32 itemHash, uint32 count)
	{
		uint32 totalDeleted = 0;
		for(std::map<int,Bag>::iterator it=bags.begin(), endIt=bags.end(); it!=endIt; ++it)
		{
			if (it->second.IsEnabled()==false) continue; // skip disabled bag

			uint32 deleted = it->second.DeleteItems(itemHash, static_cast<uint16>(count));
			totalDeleted += deleted;
			if(deleted < count)
			{
				count -= deleted;
				continue;
			}
			else
				break;
		}

		return totalDeleted;
	}

	bool Inventory::MoveItems(uint32 sBag, uint32 sPos, uint32 tBag, uint32 tPos)
	{
		if( !Exist(sBag, sPos) || !Exist(tBag, tPos ) )
		{
			LoggerInstance().Error("Inventory::MoveItems() - bags({0}) > bags.size() || tBag({1}) < bags.size()", sBag, tBag );
			return false;
		}

		Common::ItemEx src=bags[sBag].GetItem(sPos);
		Common::ItemEx tgt=bags[tBag].GetItem(tPos);

		/* swap */
		bags[sBag].SetItem(sPos, tgt);
		bags[tBag].SetItem(tPos, src);
		return true;
	}

	bool Inventory::StackItems(uint32 sBag, uint32 sPos, uint32 tBag, uint32 tPos, uint32 count)
	{
		if( !Exist(sBag, sPos) || !Exist(tBag, tPos ) )
		{
			LoggerInstance().Error("Inventory::StackItems() - bags({0}) > bags.size() || tBag({1}) > bags.size()", sBag, tBag );
			return false;
		}

		Common::ItemEx src=bags[sBag].GetItem(sPos);
		Common::ItemEx tgt=bags[tBag].GetItem(tPos);

		/* validation */
		if (count==0 || src.Info==NULL || src.StackedCount<count) return false;
		if (tgt.Info!=NULL && (src.Info!=tgt.Info || static_cast<int>(tgt.StackedCount+count)>tgt.Info->MaxCnt)) return false;

		/* stack */
		if (tgt.Info==NULL) { tgt=src; tgt.StackedCount=0; }

		src.StackedCount=src.StackedCount-static_cast<uint16>(count);
		if (src.StackedCount==0) src.Info=NULL; // clear
		tgt.StackedCount=tgt.StackedCount+static_cast<uint16>(count);

		bags[sBag].SetItem(sPos, src);
		bags[tBag].SetItem(tPos, tgt);

		return true;
	}

	void Inventory::AllItems(std::vector< Common::ItemSlotEx >& allItems)
	{
		for(std::map<int,Bag>::iterator it=bags.begin(), endIt=bags.end(); it!=endIt; ++it)
		{
			it->second.AllItems(allItems);
		}
	}
	void Inventory::GetAllItemSlots(std::vector< const std::vector<Common::ItemEx>* >& slots)
	{
		slots.clear();
		for(std::map<int,Bag>::iterator it=bags.begin(), endIt=bags.end(); it!=endIt; ++it)
		{
			slots.push_back( &it->second.GetAllItemSlots() );			
		}
	}

	void Inventory::AllItems(std::vector< XRated::ItemSlot >& allItems)
	{
		for(std::map<int,Bag>::iterator it=bags.begin(), endIt=bags.end(); it!=endIt; ++it)
		{
			it->second.AllItems(allItems);
		}
	}
	

	void Inventory::Init()
	{ // disable all bags
		for(std::map<int,Bag>::iterator it=bags.begin(), endIt=bags.end(); it!=endIt; ++it)
		{
			it->second.SetEnable(false);
			it->second.Clear();
		}
	}

	void Inventory::RenewBag(int bagNumber, const DateTime& expireDate)
	{
		Bags::const_iterator bag = bags.find( bagNumber );
		if( bag == bags.end() )
		{// this is problably wrong.
			bags[bagNumber] = Bag(bagNumber, SLOTCOUNT, ISBANK ? 1 : 0); // 0 == inventoryBag, 1 == bankBag;
		}

		bool enabled=DateTime::Now()<expireDate?true:false;
		bags[bagNumber].SetEnable(enabled);
		bags[bagNumber].ExpireDate=expireDate;
	}

	void Inventory::GetBags(std::vector<uint8> & bagIndexes) const
	{
		bagIndexes.clear();
		Bags::const_iterator it = bags.begin();
		Bags::const_iterator endIt = bags.end();
		for( ; it!=endIt; ++it )
		{
			bagIndexes.push_back( it->first );
		}
	}

	void Inventory::ResetEnable(int bagNumber)
	{
		Bags::const_iterator bag = bags.find( bagNumber );
		if( bag == bags.end() )
		{
			LoggerInstance().Error("invalid bag number({0}) requested", bagNumber);
			return;
		}
		bool enabled=DateTime::Now()<bags[bagNumber].ExpireDate?true:false;
		bags[bagNumber].SetEnable(enabled);		
	}

	DateTime Inventory::GetBagExpireDate(int bagNumber) const
	{
		Bags::const_iterator bag = bags.find( bagNumber );
		if( bag != bags.end() )
		{
			return bag->second.ExpireDate;
		}

		return DateTime(0, 0, 0, 0, 0, 0);
	}

	bool Inventory::IsEnable(int bagNumber) const
	{
		Bags::const_iterator bag = bags.find( bagNumber );
		if( bag != bags.end() )
		{
			return bag->second.IsEnabled();
		}
		return false;
	}
	bool Inventory::IsEnableItem( uint8 bagNumber, const Database::Info::ItemInfo* item ) const
	{
		Database::Info::BagInfos::BagType bagType = Database::DatabaseInstance().InfoCollections.BagInfos.GetBagType( bagNumber );
		if( item != NULL )
		{
			if( !Database::DatabaseInstance().InfoCollections.BagInfos.UsableItem( bagType, item ) )
			{
				return false;
			}
		}
		else
		{
			if( bagType != Database::Info::BagInfos::BagType::Normal )
			{
				return false;
			}
		}
		return true;
	}

	bool Inventory::SetItemInstance(uint32 bag, uint32 pos, InstanceEx instance)
	{
		if( !Exist( bag, pos ) )
		{
			LoggerInstance().Error( "Inventory::SetItemInstance({0}/{1})", bag, pos );
			return false;
		}

		const Common::ItemEx& item=bags[bag].GetItem(pos);
		if (item.Info==NULL || item.StackedCount==0)
			return false;

		return bags[bag].SetItemInstance(pos, instance);
	}
	const Common::ItemEx* Inventory::ItemInfoChange(uint8 bag,uint8 position, const Database::Info::ItemInfo* info)
	{
		if( !Exist( bag, position ) )
		{
			return NULL;
		}
		return bags[bag].ItemInfoChange(position,info);		
	}
	uint16 Inventory::GetNumberOfBags() const
	{
		return bags.size();
	}

	XRated::ItemPosition Inventory::GetUnsortedItemPosition(XRated::ItemPosition start)
	{
		/*uint8 bagIndex=start.Bag;
		uint8 posIndex=start.Position;*/
		
		XRated::ItemPosition& index = start;
		for( Bags::const_iterator bagIter=bags.begin(), bagEndIter=bags.end(); bagIter!=bagEndIter; ++bagIter )
		{
			if( bagIter->first >= index.Bag )
			{
				index.Bag = bagIter->first;
				const Bag& b=bags[index.Bag];
				if ( b.IsEnabled()==false) continue; // skip disabled bag

				for( ; index.Position < SLOTCOUNT ; ++index.Position)
				{
					const Common::ItemEx* item = GetItem( index );

					if( item != NULL) return index;
				}
			}
		}
		
		return XRated::ItemPosition::Invalid;
	}
	//XRated::ItemPosition Inventory::GetNextEmptySlot(XRated::ItemPosition start)
	//{
	//	if( start.Position -1 < SLOTCOUNT )
	//	{
	//		++start.Position;
	//	}
	//	else
	//	{
	//		if( start.Bag - 1 < bags.size() )
	//		{
	//			++start.Bag;
	//		}
	//		else
	//		{
	//			return XRated::ItemPosition::Invalid;
	//		}

	//	}
	//	return start;
	//}



} } }