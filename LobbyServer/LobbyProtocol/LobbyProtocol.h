#pragma once
#include "../../Network/Protocol.h"

namespace Lobby { namespace Protocol { namespace ToServer {

	struct Head
	{
		uint16 ProtocolVersion;
		std::vector<std::string> Checksums;
		
		NET_SERIALIZABLE
	};

	struct Auth
	{
		String AccountId;
		std::string EncryptedPassword;
		bool IsPubModuleLogin;
		char AuthString[2048];
		std::wstring Locale;

		NET_SERIALIZABLE
	};
}//ToServer
}//Protocol
}//Lobby
