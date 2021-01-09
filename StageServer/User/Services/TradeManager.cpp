#include "TradeManager.h"
#include <StageServer/User/IUserRoom.h>
#include <StageServer/User/User.h>

namespace Lunia {
	namespace XRated {
		namespace StageServer {
			TradeManager::TradeInfo::AutoLock::AutoLock(UserSharedPtr& user)
				: user(user)
			{
				user->LockItems(true);
			}

			TradeManager::TradeInfo::AutoLock::~AutoLock()
			{
				user->LockItems(false);
			}



			TradeManager::TradeInfo::TradeInfo(UserSharedPtr trader, UserSharedPtr target)
				: Trader(trader), Target(target), IsReady(false), IsConfirm(TradeInfo::ConfirmType::None), Money(0)
			{
				locker = std::make_shared<TradeManager::TradeInfo::AutoLock>(trader); // automation of 
			}

			bool TradeManager::TradeInfo::IsCriticalInvalidForExchange() const
			{
				if (Target == NULL) { 
					LoggerInstance().Error(L"no target specified({0})", Trader->GetCharacterName().c_str()); 
					return true;
				}
				if (Trader->GetMoney() < Money) { 
					LoggerInstance().Error(L"not enouhg money({0})", Trader->GetCharacterName().c_str());
					return true;
				}
				for (std::vector< std::pair<ItemPosition, Common::ItemEx> >::const_iterator slot = Slots.begin(); slot != Slots.end(); ++slot)
				{
					const Common::ItemEx* item = Trader->GetItem(slot->first);
					if (item == NULL || *item != slot->second) { 
						LoggerInstance().Error(L"invlaid slot information ({0})", Trader->GetCharacterName().c_str());
						return true;
					}
				}

				/* valid or not that serious */
				return false;
			}

			void TradeManager::SendResult(User& user, Protocol::FromServer::Trade::Results result)
			{
				Protocol::FromServer::Trade packet;
				packet.Result = result;
				user.Send(packet);
			}

			void TradeManager::Log(const Common::ItemEx& item, const User& sender, const User& receiver)
			{
				/*Logger().Write(IAdmLogger::ImportantLogger, "tradeitem", sender.GetName(), Http::Logger::Parameter()
					<< item.Info->Hash << item.Serial << item.StackedCount << item.InstanceEx << receiver.GetName());*/
			}

			void TradeManager::Log(uint32 money, const User& sender, const User& receiver)
			{
				/*Logger().Write(IAdmLogger::ImportantLogger, "trademoney", sender.GetName(), Http::Logger::Parameter()
					<< money << receiver.GetName());*/
			}

			void TradeManager::Dispatch(UserSharedPtr user, Protocol::ToServer::RequestTrade& packet)
			{
				AutoLock userLock(user->GetSyncObject());

				auto room = user->GetRoom();
				if (room == NULL) // a user who did not join yet
					return;

				UserSharedPtr target = room->GetUser(packet.serial);
				if (target == NULL || target == user || target->GetState() != User::ACTIVE || user->GetState() != User::ACTIVE) // invalid target in the room
					return;

				if (user->IsItemLocked()) { //Unable to push item in cashInven when inven is locked.
					LoggerInstance().Error(L"Unable to trade when inven is locked. [{0}/{1}]", user->GetCharacterName().c_str(), target->GetCharacterName().c_str());
					return;
				}

				AutoLock lock(cs);

				std::set<TradeInfo>::iterator connector = infos.find(user);
				if (connector != infos.end()) // the user (requester) is already on trading
				{
					Cancel(user);
					return;
				}

				std::set<TradeInfo>::iterator acceptor = infos.find(target);
				if (acceptor != infos.end()) // the target is already on trading
				{
					SendResult(*user, Protocol::FromServer::Trade::TargetOnTrade);
					return;
				}

				if (target->IsConnected() == false)
				{
					Cancel(user, Protocol::FromServer::Trade::InvalidTarget);
					return;
				}
				else if (user->IsConnected() == false) // can this be ?
				{
					Cancel(user, Protocol::FromServer::Trade::InvalidTarget);
					return;
				}
				else if ((user->GetCharacterStateFlags().IsGuestID)
					|| (target->GetCharacterStateFlags().IsGuestID) 
					)
				{
					Cancel(user, Protocol::FromServer::Trade::InvalidTarget);
					return;
				}

				/* initialize trade information */
				infos.insert(TradeInfo(user, target));
				infos.insert(TradeInfo(target, user));

				/* notice to them */
				Protocol::FromServer::RequestTrade response;

				response.serial = packet.serial;
				user->Send(response); // response to connector

				response.serial = user->GetSerial();
				target->Send(response); // response to acceptor
			}

			void TradeManager::Dispatch(UserSharedPtr user, Protocol::ToServer::AddTradeItem& packet)
			{
				AutoLock userLock(user->GetSyncObject());
				AutoLock lock(cs);

				std::set<TradeInfo>::iterator i = infos.find(user);
				const Common::ItemEx* item = user->GetItem(packet.itemposition);

				/* validation */
				if (i == infos.end() || item == NULL || item->IsTradable(user) == false)
				{
					user->CriticalError("invalid trade - additem - request");
					return;
				}
				else
				{
					if (i->IsConfirm != TradeInfo::ConfirmType::None)
					{
						user->CriticalError("Invalid Trade - Confirm state isn't None at AddTradeItem");
						return;
					}

					if (i->IsReady == true)
					{
						user->CriticalError("Invalid Trade - Already Trade Ready at AddTradeItem");
						return;
					}

					for (std::vector< std::pair<ItemPosition, Common::ItemEx> >::const_iterator slot = i->Slots.begin(); slot != i->Slots.end(); ++slot)
					{
						if (slot->first == packet.itemposition) // already existing 
						{
							user->CriticalError("already added trade-item");
							return;
						}
					}
				}

				AutoLock cs(user->GetPetDatas().GetSyncObject());
				const XRated::Pet* pet = user->GetPetDatas().GetPetData(item->Serial);
				/* ready state should be released by adding item or money */
				std::set<TradeInfo>::iterator j = infos.find(i->Target);
				j->IsReady = false;
				i->IsReady = false;

				/* add the item into a trade slot */
				i->Slots.push_back(std::pair<ItemPosition, Common::ItemEx>(packet.itemposition, *item));
				i->DropItemPositions.push_back(packet.itemposition);

				/* notice to users */
				if (Database::DatabaseInstance().InfoCollections.Pets.IsPetItem(item->Info->Hash) == true && pet != NULL)
				{
					Protocol::FromServer::AddTradePetItem response;
					response.serial = user->GetSerial();
					response.itemid = item->Info->Hash;
					response.instanceEx = item->InstanceEx;
					response.count = item->StackedCount;
					response.Pet = *pet;
					user->Send(response);
					i->Target->Send(response);
				}
				else {
					Protocol::FromServer::AddTradeItem response;
					response.serial = user->GetSerial();
					response.itemid = item->Info->Hash;
					response.instanceEx = item->InstanceEx;
					response.count = item->StackedCount;

					user->Send(response);
					i->Target->Send(response);
				}
			}

			void TradeManager::Dispatch(UserSharedPtr user, Protocol::ToServer::AddTradeMoney& packet)
			{
				AutoLock userLock(user->GetSyncObject());
				AutoLock lock(cs);

				std::set<TradeInfo>::iterator i = infos.find(user);

				if (i == infos.end() || user->GetMoney() < packet.money) // basic validation
				{
					user->CriticalError("invalid trade - addmoney - request");
					return;
				}

				/* ready state should be released by adding item or money */
				std::set<TradeInfo>::iterator j = infos.find(i->Target);
				if (j == infos.end())
				{
					user->CriticalError("invalid trade - addmoney");
					return;
				}

				if (i->IsConfirm != TradeInfo::ConfirmType::None)
				{
					user->CriticalError("Invalid Trade - Confirm state isn't None at AddTradeMoney");
					return;
				}

				if (i->IsReady == true)
				{
					user->CriticalError("Invalid Trade - Already Trade Ready at AddTradeMoney");
					return;
				}

				j->IsReady = false;
				i->IsReady = false;

				/* set the money to trade */
				i->Money = packet.money;

				/* notice to users */
				Protocol::FromServer::AddTradeMoney response;
				response.serial = user->GetSerial();
				response.money = packet.money;

				user->Send(response);
				i->Target->Send(response);
			}

			void TradeManager::Dispatch(UserSharedPtr user, Protocol::ToServer::ReadyToTrade& /*packet*/)
			{
				AutoLock userLock(user->GetSyncObject());
				AutoLock lock(cs);


				std::set<TradeInfo>::iterator i = infos.find(user);
				if (i == infos.end() || i->IsReady)
				{
					user->CriticalError("invalid trade - ready - request");
					return;
				}
				i->IsReady = true;

				std::set<TradeInfo>::iterator j = infos.find(i->Target);
				if (i->Target->IsConnected() == false)
				{
					Cancel(user, Protocol::FromServer::Trade::InvalidTarget);
					return;
				}
				else if (j->Target->IsConnected() == false)
				{
					Cancel(user, Protocol::FromServer::Trade::InvalidTarget);
					return;
				}

				if (i->IsReady && j->IsReady) /* completing the trade by exchanging */
				{
					//No Item And Money
					if (i->Slots.empty() && j->Slots.empty() && i->Money == 0 && j->Money == 0)
					{
						Cancel(user, Protocol::FromServer::Trade::Canceled);
						return;
					}

					i->IsConfirm = TradeInfo::ConfirmType::Wait;
					j->IsConfirm = TradeInfo::ConfirmType::Wait;

					Protocol::FromServer::ConfirmTrade requestConfirm;

					{
						XRated::ConfirmTradeInfo tempTradeInfo;

						tempTradeInfo.playerSerial = i->Trader->GetSerial();
						tempTradeInfo.playerName = i->Trader->GetCharacterName();
						tempTradeInfo.money = i->Money;
						for (std::vector< std::pair<ItemPosition, Common::ItemEx/*to validate*/ > >::const_iterator iter = i->Slots.begin();
							iter != i->Slots.end(); ++iter)
						{
							XRated::ConfirmTradeInfo::TradeItem tempItem;
							tempItem.hash = iter->second.Info->Hash;
							tempItem.count = iter->second.StackedCount;
							tempItem.InstanceEx = iter->second.InstanceEx;

							tempTradeInfo.items.push_back(tempItem);
						}

						requestConfirm.tradePlayers.push_back(tempTradeInfo);

					}
					{
						XRated::ConfirmTradeInfo tempTradeInfo;

						tempTradeInfo.playerSerial = i->Target->GetSerial();
						tempTradeInfo.playerName = i->Target->GetCharacterName();
						tempTradeInfo.money = j->Money;
						for (std::vector< std::pair<ItemPosition, Common::ItemEx/*to validate*/ > >::const_iterator iter = j->Slots.begin();
							iter != j->Slots.end(); ++iter)
						{
							XRated::ConfirmTradeInfo::TradeItem tempItem;
							tempItem.hash = iter->second.Info->Hash;
							tempItem.count = iter->second.StackedCount;
							tempItem.InstanceEx = iter->second.InstanceEx;

							tempTradeInfo.items.push_back(tempItem);
						}

						requestConfirm.tradePlayers.push_back(tempTradeInfo);
					}

					i->Trader->Send(requestConfirm);
					i->Target->Send(requestConfirm);

				}
				else
				{
					Protocol::FromServer::ReadyToTrade response;
					response.serial = user->GetSerial();
					i->Trader->Send(response);
					i->Target->Send(response);
				}
			}

			void TradeManager::Dispatch(UserSharedPtr user, Protocol::ToServer::CancelTrade& /*packet*/)
			{
				AutoLock userLock(user->GetSyncObject());
				Cancel(user);
			}

			void TradeManager::Dispatch(UserSharedPtr user, Protocol::ToServer::ConfirmTrade& packet)
			{
				AutoLock userLock(user->GetSyncObject());
				AutoLock lock(cs);


				std::set<TradeInfo>::iterator i = infos.find(user);
				if (i == infos.end())
				{
					if (packet.conFirm == true)
					{
						user->CriticalError("invalid trade -ConfirmTrade");
						return;
					}
					else
					{
						return;
					}
				}

				std::set<TradeInfo>::iterator j = infos.find(i->Target);
				if (i->Target->IsConnected() == false)
				{
					Cancel(user, Protocol::FromServer::Trade::InvalidTarget);
					return;
				}
				else if (j->Target->IsConnected() == false)
				{
					Cancel(user, Protocol::FromServer::Trade::InvalidTarget);
					return;
				}

				if (i->IsConfirm != TradeInfo::ConfirmType::Wait)
				{
					return;
				}

				if (packet.conFirm == false)
				{
					i->IsConfirm = TradeInfo::ConfirmType::Cancel;
					if (j->IsConfirm != TradeInfo::ConfirmType::Cancel)
					{
						Cancel(user, Protocol::FromServer::Trade::Canceled);
						return;
					}
					else
					{
						return;
					}
				}
				else
				{
					i->IsConfirm = TradeInfo::ConfirmType::Confirm;
				}


				if (i->IsConfirm == TradeInfo::ConfirmType::Confirm && j->IsConfirm == TradeInfo::ConfirmType::Confirm) /* completing the trade by exchanging */
				{
					/* more validation */
					if (i->IsCriticalInvalidForExchange() || j->IsCriticalInvalidForExchange())
					{
						/* TODO: close the connection by critical behavior */
						Cancel(user);
						return;
					}

					sortStackItem(*i);
					sortStackItem(*j);

					if (validateBlankSlot(*i) == false || validateBlankSlot(*j) == false)
					{
						Cancel(user, Protocol::FromServer::Trade::NotEnoughSlot);
						return;
					}

					/* money trade */
					if (i->Money - j->Money)
					{
						uint32 tradeMoney = (i->Money > j->Money) ? (i->Money - j->Money) : (j->Money - i->Money);
						User& dorner((i->Money > j->Money) ? *(i->Trader) : *(j->Trader));
						User& receiver((i->Money > j->Money) ? *(j->Trader) : *(i->Trader));

						/* money validation */
						if (dorner.GetMoney() < tradeMoney)
						{
							Cancel(user);
							return;
						}
						/* Money OverFlow Check */
						if ((receiver.GetMoney()) > (receiver.GetMoney() + tradeMoney)
							|| receiver.GetMoney() + tradeMoney > XRated::Constants::GoldMaxLimit)
						{
							Protocol::FromServer::Trade targetResponse;
							targetResponse.Result = Protocol::FromServer::Trade::Results::TargetGoldOverflow;

							Protocol::FromServer::Trade userResponse;
							userResponse.Result = Protocol::FromServer::Trade::Results::UserGoldOverflow;

							receiver.Send(userResponse);
							dorner.Send(targetResponse);

							infos.erase(i);
							infos.erase(j);
							return;
						}

						/* exchange money */
						dorner.SetMoney(dorner.GetMoney() - tradeMoney);
						receiver.SetMoney(receiver.GetMoney() + tradeMoney);
						Log(tradeMoney, dorner, receiver);

					}

					/* dropping items before adding items to make sure the empty slots to be added */
					for (std::vector<ItemPosition>::const_iterator itr = i->DropItemPositions.begin(); itr != i->DropItemPositions.end(); ++itr)
						i->Trader->ItemDrop(*itr);
					for (std::vector<ItemPosition>::const_iterator itr = j->DropItemPositions.begin(); itr != j->DropItemPositions.end(); ++itr)
						j->Trader->ItemDrop(*itr);

					if (addTradeItem(*i) == false || addTradeItem(*j) == false)
						return;

					/* completion - almost same as Cancel(i->Trader, Protocol::FromServer::Trade::Completed)
						, different that following code avoids search(using already searched result) and Cancel doesn't mean 'Complete' */

						/* notice the trade completion */
					Protocol::FromServer::Trade response;
					response.Result = Protocol::FromServer::Trade::Completed;
					i->Trader->Send(response);
					i->Target->Send(response);
					i->Trader->UpdateInfos();
					i->Target->UpdateInfos();

					/* release the information */
					infos.erase(i);
					infos.erase(j);
				}

			}

			void TradeManager::Cancel(UserSharedPtr user, Protocol::FromServer::Trade::Results result)
			{
				AutoLock lock(cs);

				std::set<TradeInfo>::iterator i = infos.find(user);
				if (i == infos.end()) return;

				/* notice to them */
				Protocol::FromServer::Trade response;
				response.Result = result; // canceled.

				user->Send(response);
				i->Target->Send(response);

				/* remove form trade information */
				infos.erase(i->Target);
				infos.erase(i);
			}

			void TradeManager::sortStackItem(const TradeInfo& traderInfo)
			{
				for (std::vector< std::pair<ItemPosition, Common::ItemEx> >::iterator slotItr = traderInfo.Slots.begin(); slotItr != traderInfo.Slots.end(); ++slotItr)
				{
					if (slotItr->second.Info->MaxCnt == 1)
						continue;

					std::vector< std::pair<ItemPosition, Common::ItemEx> >::iterator findItr = slotItr;
					++findItr;

					for (; findItr != traderInfo.Slots.end();)
					{
						if (slotItr->second.Info->Hash == findItr->second.Info->Hash && slotItr->second.InstanceEx == findItr->second.InstanceEx)
						{
							int totalStackCount = static_cast<int>(slotItr->second.StackedCount) + static_cast<int>(findItr->second.StackedCount);

							if (totalStackCount > slotItr->second.Info->MaxCnt)
							{
								slotItr->second.StackedCount = slotItr->second.Info->MaxCnt;
								findItr->second.StackedCount = totalStackCount - slotItr->second.Info->MaxCnt;
								break;
							}

							slotItr->second.StackedCount = totalStackCount;
							findItr = traderInfo.Slots.erase(findItr);
						}
						else
							++findItr;
					}
				}
			}

			bool TradeManager::validateBlankSlot(const TradeInfo& traderInfo) const
			{
				int needSlotCount = 0;

				std::vector<StackableItem> stackableItems;

				for (std::vector< std::pair<ItemPosition, Common::ItemEx> >::const_iterator slotItr = traderInfo.Slots.begin(); slotItr != traderInfo.Slots.end(); ++slotItr)
				{
					if (slotItr->second.Info->MaxCnt == 1)
						++needSlotCount;
					else
					{
						std::vector<StackableItem>::iterator stackItemItr = stackableItems.begin();
						for (; stackItemItr != stackableItems.end(); ++stackItemItr)
						{
							if (stackItemItr->Info->Hash == slotItr->second.Info->Hash && stackItemItr->InstanceEx == slotItr->second.InstanceEx)
							{
								stackItemItr->StackedCount += slotItr->second.StackedCount;
								break;
							}
						}

						if (stackItemItr == stackableItems.end())
							stackableItems.push_back(StackableItem(slotItr->second.Info, slotItr->second.Serial, slotItr->second.InstanceEx, slotItr->second.StackedCount));
					}
				}

				std::set<TradeInfo>::const_iterator targetItr = infos.find(traderInfo.Target);
				if (targetItr == infos.end())
					return false;

				for (std::vector<StackableItem>::iterator stackItemItr = stackableItems.begin(); stackItemItr != stackableItems.end(); ++stackItemItr)
				{
					uint32 targetTradeItemStackCount = 0;

					for (std::vector< std::pair<ItemPosition, Common::ItemEx> >::const_iterator targetSlotItr = targetItr->Slots.begin(); targetSlotItr != targetItr->Slots.end(); ++targetSlotItr)
						if (targetSlotItr->second.Info->Hash == stackItemItr->Info->Hash && targetSlotItr->second.InstanceEx == stackItemItr->InstanceEx)
							targetTradeItemStackCount += targetSlotItr->second.StackedCount;

					stackItemItr->StackedCount += targetTradeItemStackCount % stackItemItr->Info->MaxCnt;

					uint32 remainItemCount = traderInfo.Target->GetRemainStackatbleItem(stackItemItr->Info, stackItemItr->Instance, stackItemItr->StackedCount);

					needSlotCount += remainItemCount / stackItemItr->Info->MaxCnt;

					if (remainItemCount % stackItemItr->Info->MaxCnt != 0)
						++needSlotCount;
				}

				if ((static_cast<int>(traderInfo.Target->GetBlankSlotCount()) + static_cast<int>(targetItr->DropItemPositions.size())) - needSlotCount < 0)
					return false;

				return true;
			}

			bool TradeManager::addTradeItem(const TradeInfo& traderInfo)
			{
				/* adding items */
				for (std::vector< std::pair<ItemPosition, Common::ItemEx> >::const_iterator slotItr = traderInfo.Slots.begin(); slotItr != traderInfo.Slots.end(); ++slotItr)
				{
					traderInfo.Target->ItemAcquire(slotItr->second.Info->Hash, traderInfo.Target->GetSerial(), slotItr->second.StackedCount, slotItr->second.InstanceEx, slotItr->second.Serial, true);
					//const Common::ItemEx& item(*(traderInfo.Target->GetItem(pos)));

					if (Database::DatabaseInstance().InfoCollections.Pets.IsPetItem(slotItr->second.Info->Hash) == true)
					{
						XRated::GlobalSerial petSerial = 0;
						{
							AutoLock cs(traderInfo.Trader->GetPetDatas().GetSyncObject());

							const XRated::Pet* pet = traderInfo.Trader->GetPetDatas().GetPetData(slotItr->second.Serial);

							if (pet != NULL)
							{
								traderInfo.Target->AddTradedPet(*pet);
								petSerial = pet->PetSerial;
							}
						}
						if (petSerial != 0)
							traderInfo.Trader->RemoveTradedPet(petSerial);
					}

					/* logging */
					Log(slotItr->second, *traderInfo.Trader, *traderInfo.Target);
				}

				return true;
			}
		}
	}
}