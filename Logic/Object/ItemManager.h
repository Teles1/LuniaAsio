#pragma once
#include "Object.h"
#include <Info/Info.h>

//using namespace Lunia::XRated::Database::Info;

namespace Lunia { namespace XRated {	namespace Logic {

	class Item : public Object {
	public :
		const static float TIMELIMIT;
		const static float CREATETIME;
		const static float DURATION;
		const static float TRYINTERVAL;
		const static float DICETIME;
	protected :
		enum ItemState {
			CreateDelay=0,
			OwnerShip,
			Opened
		};

		Database::Info::ItemInfo* info;
		Serial owner;
		float elapsedTimeFromCreation;
		ItemData itemData;
		ItemState itemState;

		bool bTried;
		float triedTime; 

	public :
		Item(IGameStateDatabase& db, Serial onwer, Database::Info::ItemInfo* i, float3 pos);
	private:
		Item(Item& item);
	public:
		virtual ~Item();

		void operator= (Item& info) ;/*{
			this->info = info.info;
			owner = info.owner;
			elapsedTimeFromCreation = 0;
			itemData = info.itemData;
			bTried = false;
			Lunia_INFO(( L"[Item::operator =] Item class �� ���Կ����ڸ� �������� �ʽ��ϴ�." ));
		}*/

		/* IObject implementation */
		inline void SetOwnerName(const wchar_t* name) { this->itemData.OwnerId = name; }
		inline void SetPrivateItem(bool bPrivateItem) { this->itemData.PrivateItem = bPrivateItem; }
		inline void SetStackCount(uint16 stackCount) { this->itemData.StackCount = stackCount; }
		inline void SetInstance(InstanceEx instance) { this->itemData.InstanceEx = instance; }
		inline ItemData& GetItemData() { return itemData; }

		bool Get(Player*, IGameStateDatabase*);

		virtual bool Update(float dt, IGameStateDatabase* db);
	};

	class ItemManager {
	protected :
		std::map<Serial, Item*> itemList;

		Serial currentOwner;	///< ���� ���� �����ΰ�.

		float tick;

	public :
		ItemManager();
		virtual ~ItemManager() {
			std::map<Serial, Item*>::iterator i = itemList.begin(), end = itemList.end();
			for (; i != end ; ++i)
				delete (*i).second;
			itemList.clear();
		}
		void operator= (ItemManager&) {
			Logger::GetInstance().Exception(L"[ItemManager::operator =] ItemManager class.");
		}

		void Init();

		const wchar_t* NextOwner(IGameStateDatabase& db);
		const wchar_t* GetRandomPlayer(IGameStateDatabase& db, Serial& owner);
		void CreateItem(IGameStateDatabase& db, uint32 name, float3 pos, float3 direction, Serial serial, uint16 stackCount=1, InstanceEx instance=0, bool isPrivateItem=false); 
		void CreateItem(IGameStateDatabase& db, uint32 name, float3 pos, float3 direction, Serial serial, uint32 userSerial, uint16 stackCount=1, InstanceEx instance=0, bool isPrivateItem=false);
		void Update(float dt, IGameStateDatabase* db);
		Object* Retrieve(Serial serial);
		void GetObjectList(std::vector<ItemData*>& l);
		uint32 GetCnt() { return (uint32)itemList.size(); }
		void ClearItem(IGameStateDatabase* db);

		uint8 GetValidStackCount(const Database::Info::ItemInfo* info, uint8 curStackCount);
	};

}	}	}
