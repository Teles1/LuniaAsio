#pragma once
#include <StageServer/Protocol/ToServer.h>
#include <StageServer/Protocol/FromServer.h>
#include <StageServer/Common.h>

namespace Lunia {
	namespace XRated {
		namespace StageServer {
			class User;
			typedef std::shared_ptr<User> UserSharedPtr;
			class Items;
			class TradeManager
			{
			public: // packet processors
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::RequestTrade& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::AddTradeItem& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::AddTradeMoney& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::ReadyToTrade& packet);
				void Dispatch(const UserSharedPtr user, Protocol::ToServer::CancelTrade& packet);

				void Dispatch(const UserSharedPtr user, Protocol::ToServer::ConfirmTrade& packet);

			public:
				void Cancel(UserSharedPtr user, Protocol::FromServer::Trade::Results result = Protocol::FromServer::Trade::Canceled);

			private:
				static void SendResult(User& user, Protocol::FromServer::Trade::Results result);
				static void Log(const Common::ItemEx& item, const User& sender, const User& receiver);
				static void Log(uint32 money, const User& sender, const User& receiver);

			private:
				std::mutex cs;

				struct StackableItem
				{
					const Database::Info::ItemInfo* Info;
					GlobalSerial Serial;
					int64 Instance;
					InstanceEx InstanceEx;
					uint32 StackedCount;

					StackableItem(const Database::Info::ItemInfo* info, GlobalSerial serial, int64 instance, uint32 stackedCount)
						: Info(info), Serial(serial), Instance(instance), StackedCount(stackedCount)
					{
					}
				};


				struct TradeInfo
				{
					TradeInfo(UserSharedPtr trader, UserSharedPtr target);
					TradeInfo(UserSharedPtr comparer) : Trader(comparer), Target(NULL) {} ///< use only at comparing

					bool operator<(const TradeInfo& rhs) const { return (Trader < rhs.Trader); }
					bool IsCriticalInvalidForExchange() const;

					UserSharedPtr Trader;
					UserSharedPtr Target;
					mutable std::vector< std::pair<ItemPosition, Common::ItemEx/*to validate*/ > > Slots;
					mutable std::vector<ItemPosition> DropItemPositions;
					mutable uint32 Money;
					mutable bool IsReady;

					enum ConfirmType
					{
						None,
						Wait,
						Confirm,
						Cancel,
					};

					mutable ConfirmType IsConfirm;

				private: // a way to keep item lock during the trade
					/*
						lock-unlock pair could not exist in the constructor and distructor in TradeInfo,
						because destructor doesn't know what comes from copy constructor or the other constructor.
						and AutoLock class makes more general and more logical way to make sure the pair processing(open-close).
					*/
					class AutoLock
					{
					public:
						AutoLock(UserSharedPtr& user);
						~AutoLock();
					private:
						AutoLock(const AutoLock&); // makes sure to be unable to copy
						UserSharedPtr& user;
					};
					std::shared_ptr<AutoLock> locker;
				};

				void sortStackItem(const TradeInfo& traderInfo);
				bool validateBlankSlot(const TradeInfo& traderInfo) const;
				bool addTradeItem(const TradeInfo& traderInfo);

				std::set< TradeInfo > infos;
			};

		}
	}
}
