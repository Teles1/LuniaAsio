#pragma once
#include <StageServer/User/Services/GuildManager.h>
#include <StageServer/User/Services/TradeManager.h>
#include <StageServer/User/Services/PlayerStoreManager.h>
#include <StageServer/Service.h>
namespace Lunia {
	namespace XRated {
		namespace StageServer {
			struct Communicators
			{
				inline Communicators(const uint16& numberOfRooms, const std::vector<Service::SquareInfo>& squareInfos) 
					: PlayerStore(numberOfRooms, squareInfos) { }

				GuildManager Guild;
				PlayerStoreManager PlayerStore;
				TradeManager Trader;
				//OpenMarketManager openMarket;
				//CashShopManager cashShopManager;
			};
		}
	}
}