#include "ItemManager.h"
#include <Info/Info/Database.h>
#include "../GameStateDatabase.h"
#include "../State/States.h"
#include <Core/DeltaTimer.h>
#include "../LogicDB/LogicDB.h"

using namespace Lunia::XRated::Database;

namespace Lunia { namespace XRated {	namespace Logic {

	const float Item::TIMELIMIT = 30.0f;
	//const float Item::CREATETIME = 2.0f;
	const float Item::CREATETIME = 0.5f;
	const float Item::DURATION = 120.0f;
	const float Item::TRYINTERVAL = 1.0f;
	const float Item::DICETIME = 10.0f;

	Item::Item(IGameStateDatabase& db, Serial serial, Info::ItemInfo* i, float3 pos)
		: Object(Constants::ObjectType::Item)
		, info(i), owner(serial), elapsedTimeFromCreation(0)
		, itemData(objectData), bTried(false), triedTime(0)
	{
		objectData.Name = info->Id;
		objectData.NameHash = info->Hash;
		objectData.Radius = 6.0f;
		objectData.Position = pos;

		switch ( i->ItemType ) {
		case Info::ItemInfo::Type::PVPITEM : //Pvp item �� �Դµ� ���� �����̰� ����.
			if ( owner )
				itemState = ItemState::OwnerShip;
			else
				itemState = ItemState::Opened;
			break;
		default :
			itemState = ItemState::CreateDelay;
		}

		Object::Initialize(&db);
	}

	//Item::Item(Item& p)
	//	: Object(Constants::ObjectType::Item)
	//	, info(p.info), owner(p.objectData.GameObjectSerial), elapsedTimeFromCreation(0)
	//	, itemData(objectData), bTried(false), triedTime(0)
	//{
	//	Lunia_WARNING((L"Logic::Item(Item&) - Owner: {}", owner ));
	//	stageData = p.stageData;
	//	objectData.GameObjectSerial = p.objectData.GameObjectSerial;
	//	objectData.Name = p.objectData.Name;
	//	objectData.NameHash = p.objectData.NameHash;
	//	objectData.Type = p.objectData.Type;
	//	objectData.Radius = p.objectData.Radius;
	//	objectData.Position = p.objectData.Position;
	//	objectData.Direction = p.objectData.Direction;
	//}

	Item::~Item()
	{
	}

	bool Item::Get(Player* player, IGameStateDatabase* db)
	{
		if ( player->GetPlayTimePenalty() & XRated::Constants::PlayTimePenalty::Item ) { //�����ߵ������ý��� ������
			bTried = true;
			return false;
		}
		switch ( info->ItemType ) {
		case Info::ItemInfo::Type::INSTANT :
			LogicDBInstance().stateDB.AddItemState(info->Hash, player);

			// StateBudle�� �ִٸ� ���ǿ� ���� ��� bundle�� ����������.
			{
				Database::Info::StateBundleInfo::ConditionList::iterator i = info->StateBundleConditions.begin();
				for ( ; i != info->StateBundleConditions.end() ; ++i) {
					player->ProcessStateBundle(*i);
				}
			}
			return true;
		case Info::ItemInfo::Type::PVPITEM : //pvp�����濡�� ���̴� ������. ��������ó���� �־�� �ϰα�����.
			if ( player->AcquireItem(info) ) {
				db->AcquirePvpItem(player, info->Hash);
				return true;
			}
			break;
		default : //�ν���Ʈ�� �ƴϸ� �ϴ��� ������
			if ( db->AcquireItem(player, info->Hash, GetSerial(), itemData.StackCount, itemData.InstanceEx) ) {
				return true;
			} else { //�κ��� ������ ��������.
				bTried = true;
			}
		}
		return false;
	}

	bool Item::Update(float dt, IGameStateDatabase* db)
	{
		elapsedTimeFromCreation += dt;

		switch ( itemState ) {
		case ItemState::CreateDelay : //�����Ǽ� ���� ���� �� ���� ����
			if (elapsedTimeFromCreation > CREATETIME) {
				if ( !(info->Attributes.Attr & Info::ItemInfo::ATTRIBUTES::ACQUIREBYDICE) )
				{
						if ( owner )
						itemState = ItemState::OwnerShip;
					else
						itemState = ItemState::Opened;
					
				}
				else if( elapsedTimeFromCreation > DICETIME ) //Ŭ�󿡼� �ֻ��������� ���⵿�� ���� �� ���� �Ѵ�.
				{
					if ( owner )
						itemState = ItemState::OwnerShip;
					else
						itemState = ItemState::Opened;
				}
			}
			break;
		case ItemState::OwnerShip : //�������� �����ϴ� ����
			{
				Player* player = db->RetrievePlayer(owner);
				//if ( owner == NULL || player == NULL )
				//{
				//	Lunia_WARNING((L"Logic::Update() - OwnerShip State : Onwer:{} / PlayerPtr: 0x%p", owner , player));
				//}
				if (player && !player->CheckState(Database::Info::StateInfo::Type::GHOST) &&
					Lunia::Math::Length<float>(player->GetPosition()-objectData.Position) <= objectData.Radius)					
				{
					if (bTried) //�������� �̹� �õ� �߾���. �̷��� ������ ���� ��ٸ���
						return false;
					if ( Get( player, db ) )
						return true;
				} else {
					bTried = false; //������ �������� �õ��ߴ��� ���θ� ��Ÿ���� flag�� �ʱ�ȭ�Ѵ�. �ƹ��� ������ ���� ���� ����
				}
				if (!itemData.PrivateItem && elapsedTimeFromCreation > TIMELIMIT) { //privateItem�� ������� �ʴ´�! �ֳı�? Opened�� ���°� �ȹٲ�ϱ� -__-!!
					itemState = ItemState::Opened;
					itemData.OwnerId = L"";
				}
			}
			break;
		case ItemState::Opened :
			{
				if ( triedTime <= 0 ) {
					std::vector<Player*> l;
					Player* player;
					db->GetAreaObject(objectData.Position, objectData.Radius, Constants::AllianceType::AnyObjects, 0, this, l);
					if (!l.empty()) {
						int cnt = (int)l.size();
						while(cnt) {
							--cnt;
							player = l[cnt];
							if ( !player->CheckState(Database::Info::StateInfo::Type::GHOST) ) {
								if ( Get( player, db) )
									return true;
							}
						}
						triedTime = TRYINTERVAL;
					} else {
						bTried = false;
					}
				} else {
					triedTime -= dt;

					if (triedTime > TRYINTERVAL) 
						Logger::GetInstance().Warn(L"Item::Update() - TriedTime Error: {0}", triedTime);
				}
			}
			if (elapsedTimeFromCreation > DURATION)
				return true;
			break;
		default :
			return true;
		}

		return Object::Update(dt, db);
	}


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ItemManager
	ItemManager::ItemManager() : currentOwner(GameStateDatabase::PCSerialBase), tick(0)
	{
	}

	void ItemManager::Init()
	{
		std::map<Serial, Item*>::iterator i = itemList.begin(), end = itemList.end();
		for (; i != end ; ++i)
			delete (*i).second;
		itemList.clear();
	}

	const wchar_t* ItemManager::NextOwner(IGameStateDatabase& db)
	{
		Player* player = db.NextPlayer(currentOwner);

		if (!player) return NULL;//����ڰ� �ƹ��� ����.

		std::set<XRated::Serial>	itemNotNomalGainUsers;

		while(player->IsGainNormal() == false)
		{
			//	�ѹ��� ������
			if(itemNotNomalGainUsers.insert(player->GetSerial()).second == false)
				return	NULL;			

			player = db.NextPlayer(player->GetSerial());
		}

        currentOwner = player->GetSerial();
		return player->GetName().c_str();
	}
	const wchar_t* ItemManager::GetRandomPlayer(IGameStateDatabase& db, Serial& owner)
	{
		Player* player = db.GetRandomPlayer();

		if (!player) return NULL;//����ڰ� �ƹ��� ����.

		owner = player->GetSerial();
		return player->GetName().c_str();
	}

	uint8 ItemManager::GetValidStackCount(const Database::Info::ItemInfo* info, uint8 curStackCount)
	{
		if (curStackCount == 1) return curStackCount;

		///< stackCount Validation
		if (!info) return 1;
		if (info->MaxCnt <= 1) return 1;
		if (info->ItemType == Info::ItemInfo::Type::INSTANT) return 1;	///< ��ùߵ��������� 1��

		return curStackCount;
	}

	void ItemManager::CreateItem(IGameStateDatabase& db, uint32 name, float3 position, float3 direction, Serial serial, uint16 stackCount, InstanceEx instance, bool isPrivateItem)
	{
		if ( position.x < 0 || position.z < 0 ) { //���� �ۿ� �������� �����Ƿ� �ߴ�.
			Logger::GetInstance().Warn( L"[ItemManager::CreateItem] Wrong position." );
			return;
		}

		if ( itemList.find(serial) != itemList.end() ) {
			// serial �� �ߺ��Ǹ� �ȵ���.
			Logger::GetInstance().Info( L"[ItemManager::CreateItem] item �����ÿ� serial�� �ߺ��Ǿ����ϴ�." );
			return;
		}

		position.y = 0;

		Info::ItemInfo* info = DatabaseInstance().InfoCollections.Items.Retrieve(name);
		if (!info) {
			Logger::GetInstance().Info( L"[ItemManager::CreateItem] item [{}] info not founded in database.", name );
			return;
		}
		Item* item;
		
		stackCount = GetValidStackCount(info, stackCount);

		if ( (info->Attributes.Attr & Info::ItemInfo::ATTRIBUTES::ACQUIREBYTURN) 
			|| (info->Attributes.Attr & Info::ItemInfo::ATTRIBUTES::ACQUIREBYDICE) ) {
			const wchar_t* ownerName;
			Serial randomOwner;
			if( info->Attributes.Attr & Info::ItemInfo::ATTRIBUTES::ACQUIREBYDICE ) {
				isPrivateItem = true;
				ownerName = GetRandomPlayer(db, randomOwner);
			}else {
				ownerName = NextOwner(db);
			}		
			
			if (ownerName) {
				if( info->Attributes.Attr & Info::ItemInfo::ATTRIBUTES::ACQUIREBYDICE ) {
					item = new Item(db, randomOwner, info, position);
				}
				else {
					item = new Item(db, currentOwner, info, position);
				}
				item->SetSerial(serial);
				item->SetDirection(direction);
				item->SetOwnerName(ownerName);
				item->SetPrivateItem(isPrivateItem);
				item->SetStackCount(stackCount);
				item->SetInstance(instance);
				itemList[serial] = item;
				db.ItemCreated(item->GetObjectData(), ownerName, 0, stackCount, isPrivateItem);
			} else {
				item = new Item(db, 0, info, position);
				item->SetSerial(serial);
				item->SetDirection(direction);
				item->SetOwnerName(L"");
				item->SetPrivateItem(isPrivateItem);
				item->SetStackCount(stackCount);
				item->SetInstance(instance);
				itemList[serial] = item;
				db.ItemCreated(item->GetObjectData(), L"", 0, stackCount, isPrivateItem);
			}
		} else {
			item = new Item(db, 0, info, position);
			item->SetSerial(serial);
			item->SetDirection(direction);
			item->SetOwnerName(L"");
			item->SetPrivateItem(isPrivateItem);
			item->SetStackCount(stackCount);
			item->SetInstance(instance);
			itemList[serial] = item;
			db.ItemCreated(item->GetObjectData(), L"", 0, stackCount, isPrivateItem);
		}

		//�������� ���͸� �������ش�.
		{
			float2 pos( position.x, position.z );
			Sector* sector = db.GetSmallSector().GetSector( pos );
			item->SetSmallSector(sector);
			sector = db.GetBigSector().GetSector( pos );
			item->SetBigSector(sector);
		}
	}

	void ItemManager::CreateItem(IGameStateDatabase& db, uint32 name, float3 position, float3 direction, Serial serial, uint32 userSerial, uint16 stackCount, InstanceEx instance, bool isPrivateItem)
	{
		if ( itemList.find(serial) != itemList.end() ) {
			Logger::GetInstance().Info( L"[ItemManager::CreateItem] item �����ÿ� serial�� �ߺ��Ǿ����ϴ�." );
			return;
		}

		Info::ItemInfo* info = DatabaseInstance().InfoCollections.Items.Retrieve(name);
		if (!info) {
			Logger::GetInstance().Info( L"[ItemManager::CreateItem] item [{0}] info not founded in database.", name);
			return;
		}

		Player* player = db.RetrievePlayer( userSerial );
		if ( !player ) return;

		if ( position.x < 0 || position.z < 0 ) { 
			Logger::GetInstance().Warn( L"[ItemManager::CreateItem] Wrong position.");
			return;
		}

		if(info->Attributes.Attr & Info::ItemInfo::ATTRIBUTES::ACQUIREBYDICE)
			isPrivateItem = true;

		position.y = 0;

		Item* item;

		stackCount = GetValidStackCount(info, stackCount);
	
		item = new Item(db, player->GetSerial(), info, position);
		item->SetSerial(serial);
		item->SetDirection(direction);
		item->SetOwnerName(player->GetName().c_str());
		item->SetPrivateItem(isPrivateItem);
		item->SetStackCount(stackCount);
		item->SetInstance(instance);
		itemList[serial] = item;
		db.ItemCreated(item->GetObjectData(), player->GetName(), 0, stackCount, isPrivateItem);

		//�������� ���͸� �������ش�.
		{
			float2 pos( position.x, position.z );
			Sector* sector = db.GetSmallSector().GetSector( pos );
			item->SetSmallSector(sector);
			sector = db.GetBigSector().GetSector( pos );
			item->SetBigSector(sector);
		}
	}

	void ItemManager::Update(float dt, IGameStateDatabase* db)
	{
		tick += dt;
		if (tick >= 0.2f) {
			tick -= 0.2f;
		} else
			return;

		std::map<Serial, Item*>::iterator i = itemList.begin(), end = itemList.end();
		for (; i != end ;) {
			if ( (*i).second->Update(0.2f, db) ) {
                ObjectData& data = (*i).second->GetObjectData();
				db->ObjectDestroyed((*i).second, data.Type);
				delete (*i).second;
				i = itemList.erase(i);
			} else
				++i;
		}
	}

	Object* ItemManager::Retrieve(Serial serial)
	{
		std::map<Serial, Item*>::iterator i = itemList.find(serial);
		if (i != itemList.end() )
			return itemList[serial];
		return NULL;
	}

	void ItemManager::GetObjectList(std::vector<ItemData*>& l)
	{
		std::map<Serial, Item*>::iterator i = itemList.begin(), end = itemList.end();
		for (; i != end ;++i) {
			ItemData& data = (*i).second->GetItemData();
			l.push_back(&data);
		}
	}

	void ItemManager::ClearItem(IGameStateDatabase* db)
	{
		std::map<Serial, Item*>::iterator i = itemList.begin(), end = itemList.end();
		for (; i != end ; ++i) {
			ObjectData& data = (*i).second->GetObjectData();
			db->ObjectDestroyed((*i).second, data.Type);
			delete (*i).second;
		}
		itemList.clear();
	}


}	}	}

