#pragma once
#include "Network/Tcp/Server.h"
#include <Core/Utils/ConfigReader.h>
#include <StageServer/User/User.h>
namespace Lunia {
	namespace StageServer {
		struct StageServer : public Net::ServerTcp<User> {
			StageServer(const ServerAddress& address);
			void HandleNewConnection(const asio::error_code& err_code
				, asio::ip::tcp::socket& socket);
		};
	}
}