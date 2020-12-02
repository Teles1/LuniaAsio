#pragma once
#include "../../Network/Protocol.h"
#include "../Core/Utils/DateTime.h"
namespace Lobby { namespace Protocol {

	struct Head
	{
		uint16 ProtocolVersion;
		std::vector<std::string> Checksums;

		enum Results
		{
			Ok = 0,
			VersionMismatched,
			NoResponse = 0xff,
		} Result;

		DateTime ServerTime;
		std::string UserIP;
		uint32 EncryptKey;

		int Hashcode;
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

	struct Alive
	{
		uint32 Index;
		uint32 Value1;
		uint32 Value2;
		uint32 Value3;

		NET_SERIALIZABLE
	};

	struct CharacterSlots
	{
		int32 NumberOfSlots;
		std::vector<int32> CharacterLicenses;

		NET_SERIALIZABLE
	};

}//Protocol
}//Lobby
