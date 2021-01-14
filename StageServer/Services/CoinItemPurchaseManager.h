#pragma once
#include <Network/Api/Api.h>
namespace Lunia {
	namespace XRated {
		namespace StageServer {
			class User;
			typedef std::shared_ptr<User> UserSharedPtr;
			class ICoinItemPurchaseManager
			{
			public:

				enum CoinItemType
				{
					InstantCoinReviveItem = 0,
					CashItem = 1,
					count,
				};

				virtual void PurchaseCoinItem(UserSharedPtr user, const uint32 senderUniqueSerial, const uint32 roomIndex, const uint16 roomID, const CoinItemType coinItemType, uint32 productID = 0 ) = 0;
				virtual void ErasePurchaseCoinItemData(const uint16 roomID, const CoinItemType coinItemType) = 0;
				virtual void ErasePurchaseCoinItemData(const CoinItemType coinItemType , const uint64 orderNumber ) = 0;
				virtual bool UndoPurchaseCoinItem(UserSharedPtr user, const CoinItemType coinItemType , const uint64 orderNumber ) = 0;
				virtual bool UndoPurchaseCoinItem(const CoinItemType coinItemType, const uint64 orderNumber) = 0;
				virtual bool UndoPurchaseCoinItem(const CoinItemType coinItemType , const uint32 userUniqueSerial) = 0;
				virtual bool UndoPurchaseCoinItem(const uint16 roomID) = 0;
				virtual bool CheckRollbackPurchaseCoinItem(const uint64 orderNumber) = 0;
				virtual void ReportPurchasedCoinItem(std::vector< std::wstring >& reporter) = 0;

				virtual void PurchaseInstantCoin(const CoinItemType	coinItemType, const uint64 orderNumber) = 0;

				// New InstantCoinRevive
				virtual void DBCheckInstantCoin(const UserSharedPtr& user, const Net::Answer& answer) = 0;
				virtual void DBPurchaseInstantCoin(const UserSharedPtr& user, const Net::Answer& answer) = 0;

				virtual void DBPurchaseCashItem(const UserSharedPtr& user, const Net::Answer& answer) = 0;

			};

			ICoinItemPurchaseManager& CoinItemPurchaseManagerInstance();

		}
	}
}