#pragma once
#include "Core/Serializer/Serializer.h"
#include "Core/Utils/DateTime.h"
#include <Core/GameConstants.h>
namespace Lunia {
	namespace Lobby {
		namespace Protocol {

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
				int32 NumberOfSlots;				  ///< total number of characters that can exist (created - deleted)
				std::vector<int32> CharacterLicenses; ///< each elements are matched with XRated::Constants::ClassType

				NET_SERIALIZABLE;
			};

			struct ListCharacter : public Serializer::ISerializable
			{
				std::vector<XRated::LobbyPlayerInfo> Characters;

				NET_SERIALIZABLE;
			};

			//This comes from the client only.
			struct CheckSecondPassword : public Serializer::ISerializable
			{
				std::wstring AccountName;

				NET_SERIALIZABLE;
			};

			struct CreateSecondPassword : public Serializer::ISerializable
			{
				std::wstring AccountName;
				std::wstring Password;

				NET_SERIALIZABLE;
			};

			struct DeleteSecondPassword : public Serializer::ISerializable
			{
				std::wstring AccountName;
				std::wstring Password;

				NET_SERIALIZABLE;
			};

			struct ModifySecondPassword : public Serializer::ISerializable
			{
				std::wstring AccountName;
				std::wstring Password;
				std::wstring NewPassword;

				NET_SERIALIZABLE;
			};

			struct SignInSecondPassword : public Serializer::ISerializable
			{
				std::wstring AccountName;
				std::wstring Password;
				uint32 FailCount;
				std::wstring LockExpired;

				NET_SERIALIZABLE;
			};


			struct SecondPasswordChecked : public Serializer::ISerializable
			{
				enum Results
				{
					Ok = 0,
					NoResponse = 0xff
				} Result;

				NET_SERIALIZABLE;
				uint8 PasswordInUse;
				uint32 FailCount;
				std::wstring LockExpired;
				uint8 IsLocked;
			};
			
			struct SecondPasswordCreated : public Serializer::ISerializable
			{
				enum Results
				{
					Ok = 0,
					NoResponse = 0xff
				} Result;

				NET_SERIALIZABLE;
			};

			struct SecondPasswordDeleted : public Serializer::ISerializable
			{
				enum Results
				{
					Ok = 0,
					NoResponse = 0xff
				} Result;

				NET_SERIALIZABLE;
			};

			struct SecondPasswordModified : public Serializer::ISerializable
			{
				enum Results
				{
					Ok = 0,
					NoResponse = 0xff
				} Result;

				NET_SERIALIZABLE;
			};

			struct SecondPasswordSignedIn : public Serializer::ISerializable
			{
				enum Results
				{
					Ok = 0,
					InvalidPassword = 1,
					NoResponse = 0xff
				} Result;

				NET_SERIALIZABLE;
			};

		}//Protocol
	}//Lobby
}
