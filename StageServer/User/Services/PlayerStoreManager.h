#pragma once
#include <StageServer/Protocol/ToServer.h>
#include <Info/Info.h>
#include <StageServer/Service.h>
namespace Lunia {
	namespace XRated {
		namespace StageServer {
			class User;
			typedef std::shared_ptr<User> UserSharedPtr;
			class Items;
			class PlayerStoreManager
			{
			public: // packet processors
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::PlayerStore::Buy& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::PlayerStore::BuyPet& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::PlayerStore::List& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::PlayerStore::Close& packet);

			public:
				PlayerStoreManager(int numberOfRooms, std::vector<Service::SquareInfo> squares);
				void Close(UserSharedPtr user);				/// called when connection is closed

				bool IsOpened(UserSharedPtr user) const; ///< @return true if the user already opened a player shop.
				void Open(UserSharedPtr user, Items& items, const Database::Info::ItemInfo& storeItem, float commissionRate, float discountRate
					, const String& title, const std::vector<StoreSlot> slots
					, const std::vector<StoreSlotForPet> slotsForPet
				);	///< open a player store, normally it's called by UseItemEx (NOLOGIC item state)
				void SendAllPlayerStore(UserSharedPtr target); ///< send every opened player store information in the room to the target

			private:
				struct PlayerStore
				{
				public:
					uint32 StoreItemHash;
					String Title;
					std::vector< StoreSlot > Slots;
					std::vector< StoreSlotForPet > SlotsForPet;
					float3 Location;
					Items* ItemManager;
					UserSharedPtr Host;
					float CommissionRate;
					float DiscountRate;
				};
				typedef std::map<uint32 /*player serial*/, PlayerStore> PlayerStoreMap;
				std::vector< PlayerStoreMap > stores;

				std::vector<Service::SquareInfo> squareInfos;

				/** guard for thread safety by accessing member stores.
					@remarks mutable is used because IsOpened function is constant member.
					@TODO fix the store array size by number of rooms and use lock only on PlayerStoreMap.
				*/
				mutable std::mutex guard;
			};
		}
	}
}