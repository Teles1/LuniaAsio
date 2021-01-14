#include "CoinItemPurchaseManager.h"
#include <Core/ReferenceCounted.h>
#include <StageServer/Room/RoomManager.h>
#include <StageServer/User/User.h>
namespace Lunia {
    namespace XRated {
        namespace StageServer {
			class CoinItemPurchaseManager : public ICoinItemPurchaseManager
			{
			private:
				std::mutex cs;
				struct PurchaseData : public ReferenceCounted
				{
				public:
					enum PurchaseStateType
					{
						NotRequested = 0,

						PurchaseRequested = 1,
						Purchased = 2,

						RollbackRequested = 10,
						Rollbacked = 11,

						PurchaseErrored = 20,
						RollbackErrored = 21,
					};


				public:
					UserSharedPtr user;
					unsigned int		senderUniqueSerial;
					int					roomIndex;
					uint16				roomID;
					CoinItemType		coinItemType;
					uint64				orderNumber;
					PurchaseStateType	purchaseState;


					PurchaseData(UserSharedPtr pUser
						, unsigned int		pSenderUniqueSerial
						, uint32			pRoomIndex
						, uint16			pRoomID
						, CoinItemType		pCoinItemType
						, uint64			pOrderNumber)
						: user(pUser)
						, senderUniqueSerial(pSenderUniqueSerial)
						, roomIndex(pRoomIndex)
						, roomID(pRoomID)
						, coinItemType(pCoinItemType)
						, orderNumber(pOrderNumber)
						, purchaseState(PurchaseRequested)
					{
					}
				};
				typedef std::list< com_ptr< PurchaseData > > PurchaseList;

				typedef std::map< int, PurchaseList > PurchaseMappedQueue;

				PurchaseMappedQueue			purchaseMappedQueue;

			private:

				com_ptr<PurchaseData> CreatePurchaseData(UserSharedPtr user, uint32 senderUniqueSerial, uint32 roomIndex, uint16 roomID, CoinItemType coinItemType)
				{
					return new PurchaseData(user, senderUniqueSerial, roomIndex, roomID, coinItemType, 0);
				}
				
				bool CheckRollbackPurchaseCoinItem(com_ptr< PurchaseData > purchaseData)
				{
					if (purchaseData->purchaseState == PurchaseData::Purchased)
					{
						if (RoomManagerInstance().IsRoomAvailable(purchaseData->roomIndex, purchaseData->roomID) == true &&
							RoomManagerInstance().IsRoomMissionEnded(purchaseData->roomIndex) == false)
						{
							if (RoomManagerInstance().IsUserExist(purchaseData->user->GetName(), purchaseData->roomIndex) == true)
							{
								return false;
							}
							else
							{
								return true;
							}
						}
						else
						{
							return true;
						}

					}
					return false;
				}

			public:

				CoinItemPurchaseManager()
				{
					purchaseMappedQueue.clear();

					PurchaseList list;
					list.clear();

					purchaseMappedQueue[InstantCoinReviveItem] = list;
					purchaseMappedQueue[CashItem] = list;
				}

				void PurchaseCoinItem(UserSharedPtr user, const uint32 senderUniqueSerial, const uint32	roomIndex, const uint16	roomID, const CoinItemType	coinItemType, uint32 productID)
				{
					AutoLock lock(cs);
					std::string operation;

					switch (coinItemType)
					{
					case InstantCoinReviveItem:
						operation = "Allm.Shop.DirectResurrection_Check";
						break;
					case CashItem:
						operation = "Allm.Shop.Purchase";
						break;
					}

					if (operation.empty() == false)
					{
						PurchaseMappedQueue::iterator findListIter = purchaseMappedQueue.find(coinItemType);

						if (findListIter != purchaseMappedQueue.end())
						{
							Net::Api request(operation);
							request << user->GetName();

							switch (coinItemType)
							{
							case InstantCoinReviveItem:
								request.GetAsync(this, &CoinItemPurchaseManager::DBCheckInstantCoin, user);
								break;
							case CashItem:
								request << productID;
								request.GetAsync(this, &CoinItemPurchaseManager::DBPurchaseCashItem, user);
								break;
							}


							for (PurchaseList::iterator iter = findListIter->second.begin(); iter != findListIter->second.end(); ++iter)
							{
								if ((*iter)->user->GetName() == user->GetName() && (*iter)->coinItemType == coinItemType &&
									(*iter)->purchaseState == PurchaseData::PurchaseRequested)
								{
									(*iter)->purchaseState = PurchaseData::PurchaseErrored;
								}
							}


							findListIter->second.push_back(CreatePurchaseData(user, senderUniqueSerial, roomIndex, roomID, coinItemType));
							LoggerInstance().Warn(L"PurchaseCoinItem - PurchaseItem [{}] Queued total : {}", coinItemType, findListIter->second.size());
						}
						else
						{
							LoggerInstance().Error(L"PurchaseCoinItem - Cannot find coinItemType [{}]", coinItemType);
						}
					}
				}

				void PurchaseInstantCoin(const CoinItemType	coinItemType, const uint64	orderNumber)
				{
					AutoLock lock(cs);

					PurchaseMappedQueue::iterator findListIter = purchaseMappedQueue.find(coinItemType);
					if (findListIter != purchaseMappedQueue.end())
					{
						Net::Api request("Allm.Shop.DirectResurrection_Purchase");

						PurchaseList::iterator	iter;
						for (iter = findListIter->second.begin(); iter != findListIter->second.end(); ++iter)
						{
							if ((*iter)->orderNumber == orderNumber)
							{
								LoggerInstance().Warn(L"PurchaseInstantCoin - PurchaseItem [{}] Queued total : {}", coinItemType, findListIter->second.size());
								request << (*iter)->user->GetName();
								request.GetAsync(this, &CoinItemPurchaseManager::DBPurchaseInstantCoin, (*iter)->user);
								return;
							}
						}
					}
					else
					{
						LoggerInstance().Error(L"PurchaseCoinItem - Cannot find coinItemType [{}]", coinItemType);
					}
				}

				void ErasePurchaseCoinItemData(const uint16 roomID, const CoinItemType coinItemType )
				{
					AutoLock lock(cs);
					PurchaseMappedQueue::iterator		findListIter = purchaseMappedQueue.find(coinItemType);

					if (findListIter != purchaseMappedQueue.end())
					{
						PurchaseList::iterator findIter;
						for (findIter = findListIter->second.begin(); findIter != findListIter->second.end(); ++findIter)
						{
							if ((*findIter)->roomID == roomID && (*findIter)->coinItemType == coinItemType)
							{
								findListIter->second.erase(findIter);
							}
						}
					}
					else
					{
						LoggerInstance().Error(L"ErasePurchaseCoinItemData - Cannot find coinItemType [{}]", coinItemType);
					}
				}

				void ErasePurchaseCoinItemData(const CoinItemType coinItemType, const uint64 orderNumber)
				{
					AutoLock lock(cs);
					PurchaseMappedQueue::iterator		findListIter = purchaseMappedQueue.find(coinItemType);

					if (findListIter != purchaseMappedQueue.end())
					{

						PurchaseList::iterator findIter;
						for (findIter = findListIter->second.begin(); findIter != findListIter->second.end(); ++findIter)
						{
							if ((*findIter)->orderNumber == orderNumber && (*findIter)->coinItemType == coinItemType )
							{
								findListIter->second.erase(findIter);
								return; 
							}
						}

						LoggerInstance().Warn(L"ErasePurchaseCoinItemData - Failed, info not found :item. {}, order No. {}", coinItemType, orderNumber);
					}
					else
					{
					}
				}

				bool UndoPurchaseCoinItem(UserSharedPtr user, const CoinItemType coinItemType, const uint64 orderNumber )
				{
					AutoLock lock(cs);

					PurchaseMappedQueue::iterator	findListIter = purchaseMappedQueue.find(coinItemType);

					if (findListIter != purchaseMappedQueue.end())
					{
						PurchaseList::iterator	findIter;
						for (findIter = findListIter->second.begin(); findIter != findListIter->second.end(); ++findIter)
						{
							if ((*findIter)->orderNumber == orderNumber && (*findIter)->coinItemType == coinItemType)
								break;
						}

						if (findIter != findListIter->second.end())
						{
							findListIter->second.erase(findIter);
							LoggerInstance().Warn(L"UndoPurchaseCoinItem - Requested : item. {}, order No. {}", coinItemType, orderNumber);
							return true;
						}
						else
							LoggerInstance().Warn(L"UndoPurchaseCoinItem - Failed, info not found :item. {}, order No. {}", coinItemType, orderNumber);
					}
					else
						LoggerInstance().Warn(L"UndoPurchaseCoinItem - Failed, info not found :item. {}, order No. {}", coinItemType, orderNumber);

					return false;
				}

				bool UndoPurchaseCoinItem(const CoinItemType coinItemType, const uint64 orderNumber)
				{
					AutoLock lock(cs);

					PurchaseMappedQueue::iterator	findListIter = purchaseMappedQueue.find(coinItemType);

					if (findListIter != purchaseMappedQueue.end())
					{
						PurchaseList::iterator	findIter;
						for (findIter = findListIter->second.begin(); findIter != findListIter->second.end(); ++findIter)
						{
							if ((*findIter)->orderNumber == orderNumber && (*findIter)->coinItemType == coinItemType)
								break;
						}

						if (findIter != findListIter->second.end())
						{
							findListIter->second.erase(findIter);
							LoggerInstance().Warn(L"UndoPurchaseCoinItem - Requested : item. {}, order No. {}", coinItemType, orderNumber);
							return true;
						}
						else
							LoggerInstance().Warn(L"UndoPurchaseCoinItem - Failed, info not found :item. {}, order No. {}", coinItemType, orderNumber);
					}
					else
						LoggerInstance().Warn(L"UndoPurchaseCoinItem - Failed, info not found :item. {}, order No. {}", coinItemType, orderNumber);

					return false;
				}

				bool UndoPurchaseCoinItem(const CoinItemType coinItemType, const uint32 userUniqueSerial)
				{
					AutoLock lock(cs);

					PurchaseMappedQueue::iterator	findListIter = purchaseMappedQueue.find(coinItemType);

					if (findListIter != purchaseMappedQueue.end())
					{
						PurchaseList::iterator	findIter;
						for (findIter = findListIter->second.begin(); findIter != findListIter->second.end(); ++findIter)
						{
							if ((*findIter)->senderUniqueSerial == userUniqueSerial && (*findIter)->coinItemType == coinItemType)
								break;
						}

						if (findIter != findListIter->second.end())
						{
							findListIter->second.erase(findIter);
							LoggerInstance().Warn(L"UndoPurchaseCoinItem - Requested : item. {}, userUniqueSerial. {}", coinItemType, userUniqueSerial);
							return true;
						}
						else
							LoggerInstance().Warn(L"UndoPurchaseCoinItem - Failed, info not found :item. {}, userUniqueSerial. {}", coinItemType, userUniqueSerial);
					}
					else
						LoggerInstance().Warn(L"UndoPurchaseCoinItem - Failed, info not found :item. {}, userUniqueSerial. {}", coinItemType, userUniqueSerial);

					return false;
				}

				bool UndoPurchaseCoinItem(const uint16 roomID)
				{
					AutoLock lock(cs);

					PurchaseMappedQueue::iterator		listIter;

					for (listIter = purchaseMappedQueue.begin(); listIter != purchaseMappedQueue.end(); ++listIter)
					{
						PurchaseList::iterator		iter;

						for (iter = listIter->second.begin(); iter != listIter->second.end(); ++iter)
						{
							if ((*iter)->roomID == roomID)
							{
								LoggerInstance().Warn(L"UndoPurchaseCoinItem - Requested : item. {}, order No. {}", (*iter)->coinItemType, (*iter)->orderNumber);
								listIter->second.erase(iter);
							}
						}
					}

					return true;
				}

				bool CheckRollbackPurchaseCoinItem(const uint64 orderNumber)
				{
					PurchaseMappedQueue::iterator		listIter;
					PurchaseList::iterator				iter;

					for (listIter = purchaseMappedQueue.begin(); listIter != purchaseMappedQueue.end(); ++listIter)
					{
						for (iter = listIter->second.begin(); iter != listIter->second.end(); ++iter)
						{
							if ((*iter)->orderNumber == orderNumber)
							{
								LoggerInstance().Info(L"CheckRollbackPurchaseCoinItem - Search Complete, info found :order No. {}", orderNumber);
								return CheckRollbackPurchaseCoinItem((*iter));
							}
						}
					}

					LoggerInstance().Info(L"CheckRollbackPurchaseCoinItem - Search Aborted, info not found :order No. {}", orderNumber);
					return false; 	
				}

				void ReportPurchasedCoinItem(std::vector< std::wstring >& reporter)
				{
					int listCount = 0;
					int purchaseErrored = 0;
					int rollbackRequested = 0;
					int rollbackErrored = 0;
					if (purchaseMappedQueue.empty() == false)
					{
						for (PurchaseMappedQueue::iterator listIter = purchaseMappedQueue.begin();
							listIter != purchaseMappedQueue.end();
							++listIter)
						{
							std::wstring total = fmt::format(L"CoinItem Type : {}, Total : {}\n", listCount, listIter->second.size());
							reporter.push_back(total);

							for (PurchaseList::iterator iter = listIter->second.begin();
								iter != listIter->second.end();
								++iter)
							{
								std::wstring temp =
									fmt::format(L"Item : {}, User : {}, Order : {}, State : {}\n",
										(*iter)->coinItemType, (*iter)->user->GetName().c_str(), (*iter)->orderNumber, (*iter)->purchaseState);

								reporter.push_back(temp);

								if ((*iter)->purchaseState == PurchaseData::PurchaseErrored)
									purchaseErrored += 1;
								else if ((*iter)->purchaseState == PurchaseData::RollbackErrored)
									rollbackErrored += 1;
								else if ((*iter)->purchaseState == PurchaseData::RollbackRequested)
									rollbackRequested += 1;

							}
							++listCount;
						}
						std::wstring last = fmt::format(L"Purchase Errored : {}, Rollback Requested : {}, Rollback Errored : {}\n",
							purchaseErrored, rollbackRequested, rollbackErrored);
						reporter.push_back(last);

					}
					else
					{
						reporter.push_back(L"(empty list)");
					}
				}

				////////////////////////////////////////////////////////////////////////////////////////////////////////
				/// DB received.
				void DBCheckInstantCoin(const UserSharedPtr& user, const Net::Answer& answer)
				{
					AutoLock lock(cs);
					if (answer)
					{
						std::wstring characterName = answer.at("characterName").get<std::wstring>();
						if (characterName == user->GetName())
						{
							PurchaseMappedQueue::iterator findListIter = purchaseMappedQueue.find(InstantCoinReviveItem);

							if (findListIter != purchaseMappedQueue.end())
							{
								PurchaseList::iterator	findIter;

								for (findIter = findListIter->second.begin(); findIter != findListIter->second.end(); ++findIter)
								{
									if ((*findIter)->user->GetName() == user->GetName() && (*findIter)->coinItemType == InstantCoinReviveItem &&
										(*findIter)->purchaseState == PurchaseData::PurchaseRequested 
										)
										break;
								}
								if (findIter != findListIter->second.end())
								{
									(*findIter)->orderNumber = StringUtil::Hash(characterName.c_str()) + user->GetSerial();
									(*findIter)->purchaseState = PurchaseData::Purchased;

									if (CheckRollbackPurchaseCoinItem((*findIter)) == true)
									{
										UndoPurchaseCoinItem(user, (*findIter)->coinItemType, (*findIter)->orderNumber);
									}
									else
									{
										user->InstantCoinRevive(answer.errorCode, (*findIter)->orderNumber);
									}
								}
								else
								{
									LoggerInstance().Error(L"DBCheckInstantCoin - Failed, packet user name not found. CANNOT FIND INFO!:item. {}, charactername. {}",
										InstantCoinReviveItem, characterName.c_str());
								}
							}
							else
							{
								LoggerInstance().Warn(L"DBCheckInstantCoin - Failed, invalid request. CANNOT FIND COIN ITEM TYPE! :item. {}, characterName. {}",
									InstantCoinReviveItem, characterName.c_str());
							}

						}
						else
						{
							UndoPurchaseCoinItem(InstantCoinReviveItem, user->GetSerial());
						}
					}
					else
					{
						LoggerInstance().Info(L"DBCheckInstantCoin - Failed userName[{}], errorCode[{}]", user->GetName().c_str(), answer.errorCode);
						user->InstantCoinRevive(answer.errorCode, 0);
						UndoPurchaseCoinItem(InstantCoinReviveItem, user->GetSerial());
					}
				}
				
				void DBPurchaseInstantCoin(const UserSharedPtr& user, const Net::Answer& answer)
				{
					AutoLock lock(cs);
					if (answer.errorCode == 0)
					{
						uint64 orderNumber = answer.at("orderNumber").get<uint64>();///< 0 : characterName, 1: accountName, 2: oidOrder
						LoggerInstance().Warn(L"DBPurchaseInstantCoin - success, item. {}, order No. {}", InstantCoinReviveItem, orderNumber);
					}
					else
					{
						LoggerInstance().Error(L"DBPurchaseInstantCoin - Failed, userName = {},  error code = {}", user->GetName().c_str(), answer.errorCode);
					}
				}

				void DBPurchaseCashItem(const UserSharedPtr& user, const Net::Answer& answer)
				{
					AutoLock lock(cs);
					if (answer)
					{
						if (user->GetSerial() == answer.at("characterId").get<uint64>())
						{
							PurchaseMappedQueue::iterator findListIter = purchaseMappedQueue.find(CashItem);

							if (findListIter != purchaseMappedQueue.end())
							{
								PurchaseList::iterator	findIter;

								for (findIter = findListIter->second.begin(); findIter != findListIter->second.end(); ++findIter)
								{
									if ((*findIter)->user->GetName() == user->GetName() && (*findIter)->coinItemType == CashItem &&
										(*findIter)->purchaseState == PurchaseData::PurchaseRequested
										)
										break;
								}

								if (answer.errorCode == 0)
								{
									if (findIter != findListIter->second.end())
									{
										answer.get_to("orderNumber",(*findIter)->orderNumber);
										(*findIter)->purchaseState = PurchaseData::Purchased;

										if (CheckRollbackPurchaseCoinItem((*findIter)) == true)
										{
											UndoPurchaseCoinItem(user, (*findIter)->coinItemType, (*findIter)->orderNumber);
										}
										else
										{
											user->PurchaseCashItem(answer.errorCode, (*findIter)->orderNumber);
											findListIter->second.erase(findIter);
										}

									}
									else
									{
										LoggerInstance().Error(L"DBPurchaseCashItem - Failed, packet user name not found. CANNOT FIND INFO!:item. {}, order No. {}",
											CashItem, answer.at("orderNumber").get<uint64>());
									}
								}
								else
								{
									LoggerInstance().Info(L"DBPurchaseCashItem - Failed, packet user not found. Requesting rollback..");
									user->PurchaseCashItem(answer.errorCode, 0);
									findListIter->second.erase(findIter);
								}

							}
							else
							{
								LoggerInstance().Warn(L"DBPurchaseCashItem - Failed, invalid request. CANNOT FIND COIN ITEM TYPE! :item. {}, order No. {}",
									CashItem, answer.at("orderNumber").get<uint64>());
							}

						}
						else
						{
							UndoPurchaseCoinItem(CashItem, user->GetSerial());
						}

					}
				}

			};

			ICoinItemPurchaseManager& CoinItemPurchaseManagerInstance()
			{
				static CoinItemPurchaseManager m_CoinItemPurchaseManagerInstance;
				return m_CoinItemPurchaseManagerInstance;
			}
        }
    }
}
