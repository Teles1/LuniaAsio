#include "PlayerStoreManager.h"
#include <StageServer/User/User.h>
#include <Core/ErrorDefinition.h>
namespace Lunia {
	namespace XRated {
		namespace StageServer {
			void PlayerStoreManager::Dispatch(UserSharedPtr& buyer, Protocol::ToServer::PlayerStore::Buy& packet)
			{
				if (buyer->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				Protocol::FromServer::PlayerStore::Buy response;

				AutoLock lock(guard);
				AutoLock userLock(buyer->GetSyncObject());

				PlayerStoreMap& storesInRoom = stores[buyer->GetRoomIndex()];
				PlayerStoreMap::iterator i = storesInRoom.find(packet.PlayerSerial);

				do
				{
					if (i == storesInRoom.end())
					{
						response.Result = Protocol::FromServer::PlayerStore::Buy::NotExists;
						break;
					}

					std::vector< StoreSlot >& slots = i->second.Slots;

					/* check validation */
					AutoLock lock(i->second.Host->GetSyncObject()); // should be locked to prevent asynchronous access of items

					std::vector< StoreSlot >::iterator slot = std::find(slots.begin(), slots.end(), packet.Item);
					if (slot == slots.end())
					{
						response.Result = Protocol::FromServer::PlayerStore::Buy::AlreadySomeoneBought;
						break;
					}
					int priceForBuyer = static_cast<uint32>(slot->SellPrice * (1.0 - static_cast<double>(i->second.DiscountRate)));
					int priceForSeller = static_cast<uint32>(slot->SellPrice * (1.0 - static_cast<double>(i->second.CommissionRate)));

					if (buyer->GetMoney() < static_cast<uint32>(priceForBuyer))
					{
						/* TODO : Make critical */
						return;
					}
					if (buyer->IsConnected() == false || i->second.Host->IsConnected() == false)
					{
						return;
					}

					/* find trading slot */
					class Condition : public Common::ItemEx::IFindCondition
					{
					public:
						Condition(const StoreSlot& slot) : slot(slot) {}
						bool operator()(const Common::ItemEx& rhs) const
						{
							return (rhs.Info && (slot.ItemHash == rhs.Info->Hash && slot.StackedCount == rhs.StackedCount && slot.InstanceEx == rhs.InstanceEx));
						}
					private:
						StoreSlot slot;
					};
					ItemPosition posSell(i->second.ItemManager->FindItem(Condition(*slot)));
					if (posSell == ItemPosition::Invalid)
					{
						/* TODO : Make critical for seller */
						slots.erase(slot);
						response.Result = Protocol::FromServer::PlayerStore::Buy::AlreadySomeoneBought;
						break;
					}

					if ((i->second.Host->GetMoney()) > (i->second.Host->GetMoney() + priceForSeller) || (i->second.Host->GetMoney() + priceForSeller) > XRated::Constants::GoldMaxLimit)
					{
						return;
					}

					/* trading item */
					const Common::ItemEx* item = i->second.ItemManager->GetItem(posSell.Bag, posSell.Position);
					ItemPosition posBuy(buyer->ItemAdd(*item));
					if (posBuy == ItemPosition::Invalid)
					{
						/* TODO : Make critical */
						return;
					}
					else
					{
						if (Database::DatabaseInstance().InfoCollections.Pets.IsPetItem(item->Info->Hash) == true)
						{
							AutoLock cs(i->second.Host->GetPetDatas().GetSyncObject());
							const XRated::Pet* pet = i->second.Host->GetPetDatas().GetPetData(item->Serial);
							if (pet) {
								buyer->CriticalError("PlayerStore::Buy, ExistPet Buy Try", true);
								return;
							}
						}
						/* logging - should be done before item drop */
						/*Logger().Write(IAdmLogger::ImportantLogger, "playerStore", buyer->GetName(), Service::Http::Logger::Parameter()
							<< i->second.Host->GetName() << i->second.StoreItemHash
							<< item->Info->Hash << item->Serial << item->StackedCount << item->InstanceEx.Instance << item->InstanceEx.ExpireDate.ToString() << slot->SellPrice);*/

						/* real trade */
						i->second.ItemManager->DropItem(posSell);
					}

					/* calculating money */
					i->second.Host->SetMoney(i->second.Host->GetMoney() + priceForSeller);
					buyer->SetMoney(buyer->GetMoney() - priceForBuyer);

					/* making result */
					response.Result = Protocol::FromServer::PlayerStore::Buy::Ok;
					response.Item = *slot;
					response.Buyer = buyer->GetSerial();

					// packet to sender
					response.CurrentMoney = i->second.Host->GetMoney();
					response.Position = posSell;
					i->second.Host->Send(response);
					i->second.Host->UpdateInfos();

					// recover for buyer
					response.CurrentMoney = buyer->GetMoney();
					response.Position = posBuy;

					slots.erase(slot);

#pragma warning(push)
#pragma warning(disable:4127) // avoid "conditional expression is constant" warning. the constant expression is used to simplify validation by 'break'
				} while (false);
#pragma warning(pop)

				buyer->Send(response);
				buyer->UpdateInfos();

			}
			
			void PlayerStoreManager::Dispatch(UserSharedPtr& buyer, Protocol::ToServer::PlayerStore::BuyPet& packet)
			{
				if (buyer->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				Protocol::FromServer::PlayerStore::Buy response;

				AutoLock lock(guard);
				AutoLock userLock(buyer->GetSyncObject());

				PlayerStoreMap& storesInRoom = stores[buyer->GetRoomIndex()];
				PlayerStoreMap::iterator i = storesInRoom.find(packet.PlayerSerial);

				do
				{
					if (i == storesInRoom.end())
					{
						response.Result = Protocol::FromServer::PlayerStore::Buy::NotExists;
						break;
					}

					std::vector< StoreSlotForPet >& slots = i->second.SlotsForPet;

					/* check validation */
					AutoLock lock(i->second.Host->GetSyncObject()); // should be locked to prevent asynchronous access of items

					std::vector< StoreSlotForPet >::iterator slot = std::find_if(slots.begin(), slots.end(), StoreSlotForPet::Finder(packet.PetSerial, packet.Item));
					if (slot == slots.end())
					{
						response.Result = Protocol::FromServer::PlayerStore::Buy::AlreadySomeoneBought;
						break;
					}
					int priceForBuyer = static_cast<uint32>(slot->SellPrice * (1.0 - static_cast<double>(i->second.DiscountRate)));
					int priceForSeller = static_cast<uint32>(slot->SellPrice * (1.0 - static_cast<double>(i->second.CommissionRate)));

					if (buyer->GetMoney() < static_cast<uint32>(priceForBuyer))
					{
						/* TODO : Make critical */
						return;
					}
					if (buyer->IsConnected() == false || i->second.Host->IsConnected() == false)
					{
						return;
					}

					/* find trading slot */
					class Condition : public Common::ItemEx::IFindCondition
					{
					public:
						Condition(const StoreSlotForPet& slot) : slot(slot) {}
						bool operator()(const Common::ItemEx& rhs) const
						{
							return (rhs.Info && (slot.ItemHash == rhs.Info->Hash && slot.StackedCount == rhs.StackedCount && slot.InstanceEx == rhs.InstanceEx
								&& rhs.Serial == slot.PetData.PetSerial));
						}
					private:
						StoreSlotForPet slot;
					};
					ItemPosition posSell(i->second.ItemManager->FindItem(Condition(*slot)));
					if (posSell == ItemPosition::Invalid)
					{
						/* TODO : Make critical for seller */
						slots.erase(slot);
						response.Result = Protocol::FromServer::PlayerStore::Buy::AlreadySomeoneBought;
						break;
					}

					if ((i->second.Host->GetMoney()) > (i->second.Host->GetMoney() + priceForSeller) || (i->second.Host->GetMoney() + priceForSeller) > XRated::Constants::GoldMaxLimit)
					{
						return;
					}

					/* trading item */
					const Common::ItemEx* item = i->second.ItemManager->GetItem(posSell.Bag, posSell.Position);
					ItemPosition posBuy(buyer->ItemAdd(*item));
					if (posBuy == ItemPosition::Invalid) {
						/* TODO : Make critical */
						return;
					}
					else {
						if (Database::DatabaseInstance().InfoCollections.Pets.IsPetItem(item->Info->Hash) == true)
						{
							XRated::GlobalSerial serial = 0;
							{
								AutoLock cs(i->second.Host->GetPetDatas().GetSyncObject());
								const XRated::Pet* pet = i->second.Host->GetPetDatas().GetPetData(item->Serial);
								if (pet != NULL) {
									buyer->AddTradedPet(*pet);
									serial = pet->PetSerial;

									/*Logger().Write(IAdmLogger::NormalLogger, "pet-transfer", buyer->GetName(), Http::Logger::Parameter()
										<< buyer->GetName() << i->second.Host->GetName() << pet->PetSerial);*/
								}
								else {
									buyer->CriticalError("PlayerStore::BuyPet, not exist pet buy try", true);
									return;
								}
							}
							if (serial != 0) {
								i->second.Host->RemoveTradedPet(serial);
							}
						}
						/* logging - should be done before item drop */
						/*Logger().Write(IAdmLogger::ImportantLogger, "playerStore", buyer->GetName(), Service::Http::Logger::Parameter()
							<< i->second.Host->GetName() << i->second.StoreItemHash
							<< item->Info->Hash << item->Serial << item->StackedCount << item->InstanceEx.Instance << item->InstanceEx.ExpireDate.ToString() << slot->SellPrice);*/

						/* real trade */
						i->second.ItemManager->DropItem(posSell);
					}

					/* calculating money */
					i->second.Host->SetMoney(i->second.Host->GetMoney() + priceForSeller);
					buyer->SetMoney(buyer->GetMoney() - priceForBuyer);

					/* making result */
					response.Result = Protocol::FromServer::PlayerStore::Buy::Ok;
					response.Item = *slot;
					response.Buyer = buyer->GetSerial();

					// packet to sender
					response.CurrentMoney = i->second.Host->GetMoney();
					response.Position = posSell;
					i->second.Host->Send(response);
					i->second.Host->UpdateInfos();

					// recover for buyer
					response.CurrentMoney = buyer->GetMoney();
					response.Position = posBuy;

					slots.erase(slot);

#pragma warning(push)
#pragma warning(disable:4127) // avoid "conditional expression is constant" warning. the constant expression is used to simplify validation by 'break'
				} while (false);
#pragma warning(pop)

				buyer->Send(response);
				buyer->UpdateInfos();
			}

			void PlayerStoreManager::Dispatch(UserSharedPtr& user, Protocol::ToServer::PlayerStore::List& packet)
			{
				if (user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				Protocol::FromServer::PlayerStore::List response;

				AutoLock lock(guard);
				AutoLock userLock(user->GetSyncObject());

				PlayerStoreMap& storesInRoom = stores[user->GetRoomIndex()];
				PlayerStoreMap::const_iterator i = storesInRoom.find(packet.PlayerSerial);
				if (i == storesInRoom.end()) return; // nothing for invalid request
				response.PlayerSerial = packet.PlayerSerial;
				response.Slots = i->second.Slots;
				response.SlotsForPet = i->second.SlotsForPet;
				user->Send(response);
			}

			void PlayerStoreManager::Dispatch(UserSharedPtr& user, Protocol::ToServer::PlayerStore::Close& /*packet*/)
			{
				Protocol::FromServer::PlayerStore::Close response;

				AutoLock lock(guard);
				AutoLock userLock(user->GetSyncObject());

				if (user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				PlayerStoreMap& storesInRoom = stores[user->GetRoomIndex()];
				response.PlayerSerial = user->GetSerial();
				user->SendToAll(response);
				user->SetState(User::ACTIVE);

				storesInRoom.erase(response.PlayerSerial);
				/* TODO: more strict validaition for close request from a player who didn't open a store */
			}

			PlayerStoreManager::PlayerStoreManager(int numberOfRooms, std::vector<Service::SquareInfo> squares)
				: stores(numberOfRooms), squareInfos(squares)
			{
			}

			void PlayerStoreManager::Close(UserSharedPtr user)
			{
				if (user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				AutoLock lock(guard);

				stores[user->GetRoomIndex()].erase(user->GetSerial());
			}

			void PlayerStoreManager::SendAllPlayerStore(UserSharedPtr target)
			{
				if (target->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}


				Protocol::FromServer::PlayerStore::Open packet;

				AutoLock lock(guard);
				const PlayerStoreMap& storesInRoom = stores[target->GetRoomIndex()];
				for (PlayerStoreMap::const_iterator i = storesInRoom.begin(); i != storesInRoom.end(); ++i)
				{
					packet.PlayerSerial = i->second.Host->GetSerial();
					packet.StoreItemHash = i->second.StoreItemHash;
					packet.Title = i->second.Title;
					target->Send(packet);
				}
			}

			bool PlayerStoreManager::IsOpened(UserSharedPtr user) const
			{
				if (user->GetCharacterStateFlags().IsGuestID)
				{
					return false;
				}

				AutoLock lock(guard);

				const PlayerStoreMap& storesInRoom = stores[user->GetRoomIndex()];
				if (storesInRoom.find(user->GetSerial()) == storesInRoom.end()) return false;
				return true;
			}

			void PlayerStoreManager::Open(UserSharedPtr user, Items& items, const Database::Info::ItemInfo& storeItem, float commissionRate, float discountRate
				, const String& title, const std::vector<StoreSlot> slots
				, const std::vector<StoreSlotForPet> slotsForPet
			)
			{
				if (user->GetCharacterStateFlags().IsGuestID)
				{
					return;
				}

				const Database::Info::StageGroup* stageGroup(Database::DatabaseInstance().InfoCollections.StageGroups.Retrieve(user->GetCurrentStage().StageGroupHash));
				if (stageGroup == NULL) return; // how it could be ?

				{
					AutoLock lock(guard);
					if (squareInfos.size() > user->GetRoomIndex() && squareInfos[user->GetRoomIndex()].maxPlayerStore < stores[user->GetRoomIndex()].size())
					{
						Protocol::FromServer::Error packet;
						packet.errorcode = XRated::Errors::TooManyPlayerStores;
						user->Send(packet);
						return;
					}

					PlayerStore store;
					store.Host = user;
					store.ItemManager = &items;

					store.Location = user->GetPlayer()->GetPosition();
					store.Slots = slots;
					store.SlotsForPet = slotsForPet;
					store.StoreItemHash = storeItem.Hash;
					store.CommissionRate = commissionRate;
					store.DiscountRate = discountRate;
					store.Title = title;

					stores[user->GetRoomIndex()][user->GetSerial()] = store;
					user->SetState(User::TRADE);
				}

				Protocol::FromServer::PlayerStore::Open packet;
				packet.PlayerSerial = user->GetSerial();
				packet.StoreItemHash = storeItem.Hash;
				packet.Title = title;
				user->SendToAll(packet);
			}
		}
	}
}