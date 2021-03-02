#pragma once
#include <Core/GameConstants.h>
#include <Network/CommonProtocol/Protocol.h>

namespace Lunia {
	namespace XRated {
		namespace Lobby {
			namespace Protocol {
				/// packets from lobby client.
				namespace ToServer
				{

					struct VarifySelf : public Serializer::ISerializable {
						std::wstring serverName;
						NET_SERIALIZABLE;
					};
					/* general notification ******************************************************************/
					/// Alive must send to server in 30 seconds after last sending any packet to server.
					struct Alive : public Serializer::ISerializable
					{
						NET_SERIALIZABLE;

						/* game guard units */
						uint32 Index;
						uint32 Value1;
						uint32 Value2;
						uint32 Value3;
					};

					/****************************************************************** general notification */

					/* initial information *******************************************************************/
					struct Head : public Serializer::ISerializable
					{
						uint16 ProtocolVersion;				///< version information of major side to control critical versioning
						std::vector<std::string> Checksums; ///< version and security checksums for client code files.

						NET_SERIALIZABLE;
					};

					struct Auth : public Serializer::ISerializable
					{
						String AccountId;
						std::string EncryptedPassword; ///< MD5 encrypted password
						bool IsPubModuleLogin;
						char AuthString[2048];
						std::wstring Locale;

						NET_SERIALIZABLE;
					};
					/**	***************************************************************** initial information */

					/* character management ******************************************************************/
					struct SelectCharacter : public Serializer::ISerializable
					{
						String Name; ///< to be selected

						NET_SERIALIZABLE;
					};

					struct DeselectCharacter : public Serializer::ISerializable
					{
						NET_SERIALIZABLE;
					};

					struct CreateCharacter : public Serializer::ISerializable
					{
						String Name;
						Constants::ClassType ClassType;

						NET_SERIALIZABLE;
					};

					struct DeleteCharacter : public Serializer::ISerializable
					{
						String Name;
						String AccountId;
						String SocialSecretNumber;
						bool IsNexonLogin;

						NET_SERIALIZABLE;
					};

					/****************************************************************** character management */

					/* stage lobby ***************************************************************************/
					struct QuickJoinStage : public Serializer::ISerializable
					{
						StageLocation Stage;

						NET_SERIALIZABLE;
					};

					struct CreateStage : public Serializer::ISerializable
					{
						StageLocation Stage;
						String Password;

						NET_SERIALIZABLE;
					};

					struct JoinStage : public Serializer::ISerializable
					{
						String Password;

						NET_SERIALIZABLE;
					};

					struct JoinInvitedStage : public Serializer::ISerializable
					{
						String CharacterName;
						StageLocation InvitedStage;
						NET_SERIALIZABLE;
					};

					///	This protocol is not using in game
					struct JoinStageAsParty : public Serializer::ISerializable
					{
						StageLocation Stage;
						String PartyCode;

						NET_SERIALIZABLE;
					};

					struct ListSquareStatus : public Serializer::ISerializable
					{
						NET_SERIALIZABLE;
					};

					struct JoinSquare : public Serializer::ISerializable
					{
						String SquareName;

						NET_SERIALIZABLE;
					};
					/*************************************************************************** stage lobby */

					/* community *****************************************************************************/
					///	Not Using below community protocol, exclude Chat(This protocol use only system message)
					/**
							For AllM messenger service using GSP and Taiwan
							*/
					struct AllMCommunity
					{
						struct ListFriends : public Serializer::ISerializable
						{
							NET_SERIALIZABLE;

							String CharacterName;
						};

						struct AddFriend : public Serializer::ISerializable
						{
							NET_SERIALIZABLE;

							String CharacterName;
							bool IsFriend;
						};

						struct RemoveFriend : public Serializer::ISerializable
						{
							NET_SERIALIZABLE;

							String CharacterName;
						};
					};
					/***************************************************************************** community */

					/* TeleSafeKey (CheckTel service) interaction for Taiwan ******************************************************************/
					struct TeleSafekeyStatus : public Serializer::ISerializable
					{
						String AccountId;

						NET_SERIALIZABLE;
					};

					/***************************************************************************** community */

					/* the others ****************************************************************************/
					struct Terminate : public Serializer::ISerializable
					{
						NET_SERIALIZABLE;
					};
					/**************************************************************************** the others */

					/* Neo Pvp Service****************************************************************************/
					struct ListPvpChannels : public Serializer::ISerializable
					{
						NET_SERIALIZABLE;
					};

					struct JoinPvpChannel : public Serializer::ISerializable
					{
						std::wstring ChannelName;
						NET_SERIALIZABLE;
					};
					/* Neo Pvp Service****************************************************************************/

					/* Security Card Check ****************************************************************************/
					struct AnswerSecurityCode : public Serializer::ISerializable
					{
						NET_SERIALIZABLE;

						std::vector<int /*Code*/> Codes;
					};
					/* Security Card Check ****************************************************************************/

					struct SaveKeySetting : public Serializer::ISerializable
					{
						NET_SERIALIZABLE;
						std::wstring Account;
						std::wstring Keycodes;
					};
					/* 3.1 by Robotex */

					struct CheckSecondPassword : public Serializer::ISerializable
					{
						NET_SERIALIZABLE;
						std::wstring AccountName;
					};

					struct CreateSecondPassword : public Serializer::ISerializable
					{
						NET_SERIALIZABLE;
						std::wstring AccountName;
						std::wstring Password;
					};

					struct DeleteSecondPassword : public Serializer::ISerializable
					{
						NET_SERIALIZABLE;
						std::wstring AccountName;
						std::wstring Password;
					};

					struct ModifySecondPassword : public Serializer::ISerializable
					{
						NET_SERIALIZABLE;
						std::wstring AccountName;
						std::wstring Password;
						std::wstring NewPassword;
					};

					struct SignInSecondPassword : public Serializer::ISerializable
					{
						NET_SERIALIZABLE;
						std::wstring AccountName;
						std::wstring Password;
						uint32 FailCount;
						std::wstring LockExpired;
					};
				} // namespace ToServer

				/** packets from lobby server.
					@remarks every "Result" member value indicates success by 0, and the other number means failure.
					*/
				namespace FromServer
				{

					/* general notification ******************************************************************/
					struct Error : public Serializer::ISerializable
					{
						enum Messages
						{
							NoError = 0,
							InvalidPCRoom,
							OutOfIpAddress,
							NoticePcRoomTime, ///< parameter will be time in minutes
							WarnPcRoomTime,   ///< parameter will be time in minutes
							InvalidClient,	///< when gameguard authorization is failed
							OutOfCapacity,	///< Too many users in lobby
						} Message;

						String Parameter;

						NET_SERIALIZABLE;
					};

					struct Alive : public Serializer::ISerializable
					{
						NET_SERIALIZABLE;

						/* game guard units */
						uint32 Index;
						uint32 Value1;
						uint32 Value2;
						uint32 Value3;
					};
					/****************************************************************** general notification */

					/* initial information *******************************************************************/
					struct Head : public Serializer::ISerializable ///< response by ToServer::Head
					{
						enum Results
						{
							Ok = 0,
							VersionMismatched,
							NoResponse = 0xff,
						} Result;

						DateTime ServerTime;
						std::string UserIP;
						uint32 EncryptKey;

						NET_SERIALIZABLE;
					};

					struct Auth : public Serializer::ISerializable ///< response by ToServer::
					{
						enum Results
						{
							Ok = 0,
							AccountNotExist = 1,
							PasswordMismatch = 2,
							DeletedAccount = 3,
							BlockedAccount = 4,
							AlreadyConnectedAtLobby = 5,
							AlreadyConnectedAtStage = 6

							,
							ServerConnectionOverflow = 9,
							InvalidIDorPW = 10,
							NullID = 11,
							NullPW = 12,
							NotInputIP = 13,
							LowerBoundID = 14,
							LowerBoundPW = 15,
							LowerBoundIP = 16,
							InvalidFirst2CharOfID = 18,
							InternalErrorSearchPW = 19

							//
							,
							SecurityCardError = 21

							,
							NoResponse = 0xff,
						} Result;
						String AccountId;
						int IDNumber;

						NET_SERIALIZABLE;
					};

					struct CharacterSlots : public Serializer::ISerializable
					{
						NET_SERIALIZABLE;

						int32 NumberOfSlots;				  ///< total number of characters that can exist (created - deleted)
						std::vector<int32> CharacterLicenses; ///< each elements are matched with XRated::Constants::ClassType
					};
					/******************************************************************* initial information */

					/* character management ******************************************************************/
					struct ListCharacter : public Serializer::ISerializable
					{
						std::vector<LobbyPlayerInfo> Characters;

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
						CharacterStateFlags CharacterStates;

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

						LobbyPlayerInfo CharacterInfo;

						NET_SERIALIZABLE;
					};

					struct DeleteCharacter : public Serializer::ISerializable
					{
						enum Results
						{
							Ok = 0,
							NotExistingCharacterName = 1,
							GuildMasterCouldNotBeDeleted = 2,
							GuildMemberCouldNotBeDeleted = 3 //Only in Allm guild system
							,
							WrongSocialSecretNumber = 4,
							NoResponse = 0xff,
						} Result;
						String DeletedCharacter;

						NET_SERIALIZABLE;
					};

					struct SocialSecretNumber : public Serializer::ISerializable
					{
						enum Results
						{
							Ok = 0,
							WronfSocialSecretNumber = 1,
							NoResponse = 0xff,
						} Result;

						NET_SERIALIZABLE;
					};
					/****************************************************************** character management */

					/* stage lobby ***************************************************************************/
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
							ServerTooBusy = 13,
						} Result;

						std::string ServerIp;
						uint16 Port;
						std::string KeyCode;

						NET_SERIALIZABLE;
					};

					struct ListSquareStatus : public Serializer::ISerializable
					{
						enum Results
						{
							Ok = 0,
							NoResponse = 0xff,
						} Result;

						std::vector<SquareInfo> SuqareList;

						NET_SERIALIZABLE;
					};
					/*************************************************************************** stage lobby */

					/* community *****************************************************************************/
					struct UpdateFriend : public Serializer::ISerializable ///< except memo information update
					{
						enum UpdatedFalgs
						{
							Level = 0x01,
							CurrentLocation = 0x02,
							OnlineState = 0x04,
						} UpdatedItem;

						Friend Friend;

						NET_SERIALIZABLE;
					};

					struct Chat : public Serializer::ISerializable
					{
						Constants::ChatTypes ChatType;
						String OriginName;
						String Message;

						NET_SERIALIZABLE;
					};

					/**
							For AllM messenger service using GSP and Taiwan
							*/
					struct AllMCommunity
					{
						struct ListFriends : public Serializer::ISerializable
						{
							NET_SERIALIZABLE;

							struct Friend : public Serializer::ISerializable
							{
								String CharacterName;
								Constants::ClassType ClassType;
								uint16 Level;
								bool IsFriend;

							public:
								void Serialize(Serializer::IStreamWriter& out) const;
								void Deserialize(Serializer::IStreamReader& in);
							};

							std::vector<Friend> FriendList;
							std::vector<std::wstring> ReferencedList;
						};

						struct AddFriend : public Serializer::ISerializable
						{
							NET_SERIALIZABLE;

							enum Results
							{
								Ok = 0,
								TargetUserNotFound = 1,
								TooManyFriends = 2
							} Result;
							String CharacterName;
							bool IsFriend;
						};

						struct RemoveFriend : public Serializer::ISerializable
						{
							NET_SERIALIZABLE;

							enum Results
							{
								Ok = 0,
								TargetUserNotFound = 1
							} Result;
							String CharacterName;
						};
					};
					/***************************************************************************** community */

					/* TeleSafeKey (CheckTel service) interaction for Taiwan ******************************************************************/
					struct TeleSafekeyStatus : public Serializer::ISerializable
					{
						enum Results
						{
							Ok = 0,
							FailedWithNoAuth = 1,
							FailedWithConnection = 2,
							FailedWithTimeout = 3,
							NoResponse = 0xff,
						} Result;

						String Response;

						NET_SERIALIZABLE;
					};

					/* termination ***************************************************************************/
					struct Terminate : public Serializer::ISerializable
					{
						enum Results
						{
							Ok = 0,
							NoResponse = 0xff,
						} Result;

						NET_SERIALIZABLE;
					};
					/*************************************************************************** termination */

					/* Neo Pvp Service****************************************************************************/
					struct ListPvpChannels : public Serializer::ISerializable
					{
						struct ResultType
						{
							enum type
							{
								Ok = 0,
								NoResponse = 0xff,
							};
						};
						ResultType::type Result;
						std::vector<PvpChannelInfo> Channels;
						NET_SERIALIZABLE;
					};

					struct JoinPvpChannel : public Serializer::ISerializable
					{
						struct ResultType
						{
							enum type
							{
								Ok = 0,
								NoResponse = 0xff,
								UserAlreadyConnected = 1,
								WrongChannelName = 2
							};
						};

						ResultType::type Result;
						std::string ServerIp;
						uint16 Port;
						std::wstring KeyCode;

						NET_SERIALIZABLE;
					};
					/* Neo Pvp Service****************************************************************************/

					/* Security Card Check ****************************************************************************/
					struct AskSecurityCode : public Serializer::ISerializable
					{
						NET_SERIALIZABLE;

						std::vector<std::pair<int /*row*/, int /*col*/> > Questions;
					};
					/* Security Card Check ****************************************************************************/

					/* LoadKeySetting ******************************************************************************/
					struct LoadKeySetting : public Serializer::ISerializable
					{
						NET_SERIALIZABLE;
						std::wstring KeyCodes;
					};
					/* // LoadKeySetting ***************************************************************************/
					/* 3.1 by Robotex */
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
					struct AchievementServerAssigned : public Serializer::ISerializable {
						NET_SERIALIZABLE;

						struct Result
						{
							enum Type
							{
								Ok = 0,
								InvalidPacket = 999
							};
						};
						Result::Type Result;
						std::wstring ServerName;
						Net::Protocol::Address serverAddress;
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
					struct FindServer : public Serializer::ISerializable {
						std::wstring characterName;
						std::wstring serverName;
						Net::Protocol::Address serverAddress;
						NET_SERIALIZABLE;
					};

					struct VarifySelf : public Serializer::ISerializable {
						std::wstring serverName;
						NET_SERIALIZABLE;
					};

					/*
					struct ServerAddress : public Serializer::ISerializable{
						std::wstring serverIpAdress;
						uint32 serverPort;
						NET_SERIALIZABLE;
					}; */
				} // namespace FromServer
			}//Protocol
		}//Lobby
	}
}
