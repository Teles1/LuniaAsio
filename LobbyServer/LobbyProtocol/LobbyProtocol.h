#pragma once
#include <Core/GameConstants.h>
#include <Network/CommonProtocol/Protocol.h>

namespace Lunia {
	namespace XRated {
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
					String Locale;
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
					String AccountName;

					NET_SERIALIZABLE;
				};

				struct CreateSecondPassword : public Serializer::ISerializable
				{
					String AccountName;
					String Password;

					NET_SERIALIZABLE;
				};

				struct DeleteSecondPassword : public Serializer::ISerializable
				{
					String AccountName;
					String Password;

					NET_SERIALIZABLE;
				};

				struct ModifySecondPassword : public Serializer::ISerializable
				{
					String AccountName;
					String Password;
					String NewPassword;

					NET_SERIALIZABLE;
				};

				struct SignInSecondPassword : public Serializer::ISerializable
				{
					String AccountName;
					String Password;
					uint32 FailCount;
					String LockExpired;

					NET_SERIALIZABLE;
				};

				//goes from server to client.
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
					String LockExpired;
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
						InvalidPassword = 3,
						NoResponse = 0xff
					} Result;

					NET_SERIALIZABLE;
				};

				struct CreateCharacter : public Serializer::ISerializable
				{
					enum Results
					{
						Ok = 0,
						AlreadyExistingCharacterName = 1,
						DeletingCharacterName = 2,
						WrongCharacterName = 3,
						NotCreateCharacterPeriod = 6,
						NoResponse = 0xff,
					} Result;

					XRated::LobbyPlayerInfo CharacterInfo;

					String Name;
					XRated::Constants::ClassType ClassType;

					NET_SERIALIZABLE;
				};

				struct Terminate : public Serializer::ISerializable
				{
					enum Results
					{
						Ok = 0,
						NoResponse = 0xff,
					} Result;

					NET_SERIALIZABLE;
				};

				struct DeleteCharacter : public Serializer::ISerializable
				{
					enum Results
					{
						Ok = 0,
						NotExistingCharacterName = 1,
						GuildMasterCouldNotBeDeleted = 2,
						GuildMemberCouldNotBeDeleted = 3, //Only in Allm guild system
						WrongSocialSecretNumber = 4,
						NoResponse = 0xff,
					} Result;
					String Name;
					String DeletedCharacter;

					NET_SERIALIZABLE;
				};

				struct SaveKeySetting : public Serializer::ISerializable
				{
					String Account;
					std::wstring Keycodes;

					NET_SERIALIZABLE;
				};

				struct SelectCharacter : public Serializer::ISerializable
				{
					enum Results
					{
						Ok = 0,
						InvalidAccount,
						Blocked,
						NotExistingCharacter,
						InvalidCharacter,
						AlreadySelectedCharacter,
						NotPhoneAuthed,

						NoResponse = 0xff,
					} Result;
					String SelectedCharacter;
					XRated::CharacterStateFlags CharacterStates;

					//client request dasopikosidpajdsjiaosdaiojsdajiodsaoji321oi309213211!!!!!!!!
					String CharacterName;
					NET_SERIALIZABLE;
				};

				struct AchievementServerAssigned : public Serializer::ISerializable {
					struct Result
					{
						enum Type
						{
							Ok = 0,
							Unknown = 1,
							InvalidPacket = 999
						};
					};
					Result::Type Result;
					std::wstring ServerName;
					Net::Protocol::Address serverAddress;
					NET_SERIALIZABLE;
				};

				struct DeselectCharacter : public Serializer::ISerializable
				{
					enum Results
					{
						Ok = 0,
						NoResponse = 0xff,
					} Result;

					NET_SERIALIZABLE;
				};

				struct ListSquareStatus : public Serializer::ISerializable
				{
					enum Results
					{
						Ok = 0,
						NoResponse = 0xff,
					} Result;

					std::vector<XRated::SquareInfo> SuqareList;

					NET_SERIALIZABLE;
				};

				struct JoinSquare : public Serializer::ISerializable
				{
					String SquareName;

					NET_SERIALIZABLE;
				};

				struct Join : public Serializer::ISerializable
				{
					enum Results
					{
						Ok = 0,
						NoResponse = 0xff,
						UserAlreadyInGame = 1,
						AlreadyExistingStage = 2,
						WrongStageName = 3,
						StageNotFound = 4,
						StageIsLocked = 5,
						StageIsFull = 6,
						UnableToAutodetect = 7,
						ServerNotFound = 8,
						ServerTooBusy = 13
					} Result;

					std::string ServerIp;
					uint16 Port;
					std::string KeyCode;

					NET_SERIALIZABLE;
				};

			}//Protocol
		}//Lobby
	}
}
