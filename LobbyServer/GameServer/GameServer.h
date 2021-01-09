#pragma once
#include "NetworkIO/GameServerNetworkIO.h"
#include <Core/Utils/ConfigReader.h>

namespace Lunia{
	namespace XRated {
		template<typename TServerProxy>
		class GameServer : public GameServerNetworkIO
		{
		public:
			inline GameServer(const ServerAddressStruct& serverAddress) : GameServerNetworkIO(serverAddress.ServerIp.c_str(), serverAddress.ServerPort)
			{
				// Proxy = new TServerProxy();

				OnSocketConnectionCreated.Connect([Proxy = &Proxy](const asio::error_code& err_code, asio::ip::tcp::socket& socket)
					{
						Proxy->ClientRegistry.MakeClient(std::move(socket)); /* WARN socket is moved a shit ton of times here */
					});
			};

			~GameServer() { };

		public:
			TServerProxy Proxy;

		private:
			/*
				TODO make fwPacketListener use the () operator instead of ::Invoke
			*/
		};
	}
}
