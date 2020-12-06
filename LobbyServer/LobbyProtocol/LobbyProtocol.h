#pragma once
#include "../Core/Core.h"
#include "../Core/Serializer/Serializer.h"
#include "../Core/Utils/DateTime.h"
namespace Lobby { namespace Protocol {

	struct Head : public Serializer::ISerializable
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

	struct Auth : public Serializer::ISerializable
	{
		enum Results
		{
			Ok = 0,
			AccountNotExist = 1,
			PasswordMismatch = 2,
			DeletedAccount = 3,
			BlockedAccount = 4,
			AlreadyConnectedAtLobby = 5,
			AlreadyConnectedAtStage = 6,

			ServerConnectionOverflow = 9,
			InvalidIDorPW = 10,
			NullID = 11,
			NullPW = 12,
			NotInputIP = 13,
			LowerBoundID = 14,
			LowerBoundPW = 15,
			LowerBoundIP = 16,
			InvalidFirst2CharOfID = 18,
			InternalErrorSearchPW = 19,
			SecurityCardError = 21,

			NoResponse = 0xff,
		} Result;

		String AccountId;
		std::string EncryptedPassword;
		bool IsPubModuleLogin;
		char AuthString[2048];
		std::wstring Locale;
		int IDNumber;

		NET_SERIALIZABLE
	};

	struct Alive : public Serializer::ISerializable
	{
		uint32 Index;
		uint32 Value1;
		uint32 Value2;
		uint32 Value3;

		NET_SERIALIZABLE
	};

	struct CharacterSlots : public Serializer::ISerializable
	{
		int32 NumberOfSlots;
		std::vector<int32> CharacterLicenses;

		NET_SERIALIZABLE
	};

}//Protocol
}//Lobby