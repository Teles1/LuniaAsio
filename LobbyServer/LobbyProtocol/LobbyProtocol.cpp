#pragma once
#include "LobbyProtocol.h"
namespace Lunia {
	namespace XRated {
		namespace Lobby {
			namespace Protocol {
				namespace ToServer
				{

					/*
						Deserialize are the ones which the server uses to read the packet coming from the client.

						Teles 3.1
					*/

					const wchar_t* VarifySelf::TypeName = L"VarifySelf";
					const HashType VarifySelf::TypeHash = StringUtil::Hash(VarifySelf::TypeName);
					void VarifySelf::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(TypeName);
						out.Write(L"serverName", serverName);
					}
					void VarifySelf::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(TypeName);
						in.Read(L"serverName", serverName);
					}

					/* ToServer::Alive */
					const wchar_t* Alive::TypeName = L"Alive";
					const HashType Alive::TypeHash = StringUtil::Hash(Alive::TypeName);
					void Alive::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(TypeName);
						out.Write(L"Index", Index);
						out.Write(L"Value1", Value1);
						out.Write(L"Value2", Value2);
						out.Write(L"Value3", Value3);
					}
					void Alive::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(TypeName);
						in.Read(L"Index", Index);
						in.Read(L"Value1", Value1);
						in.Read(L"Value2", Value2);
						in.Read(L"Value3", Value3);
					}

					/* ToServer::Head */
					const wchar_t* Head::TypeName = L"Head";
					const HashType Head::TypeHash = StringUtil::Hash(Head::TypeName);
					void Head::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(TypeName);
						out.Write(L"ProtocolVersion", ProtocolVersion);
						out.Write(L"Checksums", Checksums);
					}
					void Head::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(TypeName);
						in.Read(L"ProtocolVersion", ProtocolVersion);
						in.Read(L"Checksums", Checksums);
					}

					/* ToServer::Auth */
					const wchar_t* Auth::TypeName = L"Auth";
					const HashType Auth::TypeHash = StringUtil::Hash(Auth::TypeName);
					void Auth::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(TypeName);
						out.Write(L"AccountId", AccountId);
						out.Write(L"EncryptedPassword", EncryptedPassword);
						out.Write(L"AuthString", AuthString);
						out.Write(L"IsPubModuleLogin", IsPubModuleLogin);
						out.Write(L"Locale", Locale);
					}
					void Auth::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(TypeName);
						in.Read(L"AccountId", AccountId);
						in.Read(L"EncryptedPassword", EncryptedPassword);
						in.Read(L"AuthString", AuthString, 2048);
						in.Read(L"IsPubModuleLogin", IsPubModuleLogin);
						in.Read(L"Locale", Locale);
					}

					/* ToServer::Terminate */
					const wchar_t* Terminate::TypeName = L"Terminate";
					const HashType Terminate::TypeHash = StringUtil::Hash(Terminate::TypeName);
					void Terminate::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(TypeName);
					}
					void Terminate::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(TypeName);
					}

					/* ToServer::SelectCharacter */
					const wchar_t* SelectCharacter::TypeName = L"SelectCharacter";
					const HashType SelectCharacter::TypeHash = StringUtil::Hash(SelectCharacter::TypeName);
					void SelectCharacter::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(TypeName);
						out.Write(L"Name", Name);
					}
					void SelectCharacter::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(TypeName);
						in.Read(L"Name", Name);
					}

					/* ToServer::DeselectCharacter */
					const wchar_t* DeselectCharacter::TypeName = L"DeselectCharacter";
					const HashType DeselectCharacter::TypeHash = StringUtil::Hash(DeselectCharacter::TypeName);
					void DeselectCharacter::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(TypeName);
					}
					void DeselectCharacter::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(TypeName);
					}

					/* ToServer::CreateCharacter */
					const wchar_t* CreateCharacter::TypeName = L"CreateCharacter";
					const HashType CreateCharacter::TypeHash = StringUtil::Hash(CreateCharacter::TypeName);
					void CreateCharacter::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(TypeName);
						out.Write(L"Name", Name);
						out.Write(L"ClassType", static_cast<int>(ClassType));
					}
					void CreateCharacter::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(TypeName);
						in.Read(L"Name", Name);
						in.Read(L"ClassType", reinterpret_cast<int&>(ClassType));
					}

					/* ToServer::DeleteCharacter */
					const wchar_t* DeleteCharacter::TypeName = L"DeleteCharacter";
					const HashType DeleteCharacter::TypeHash = StringUtil::Hash(DeleteCharacter::TypeName);
					void DeleteCharacter::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(TypeName);
						out.Write(L"Name", Name);
					}
					void DeleteCharacter::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(TypeName);
						in.Read(L"Name", Name);
					}

					/* ToServer::QuickJoinStage */
					const wchar_t* QuickJoinStage::TypeName = L"QuickJoinStage";
					const HashType QuickJoinStage::TypeHash = StringUtil::Hash(QuickJoinStage::TypeName);
					void QuickJoinStage::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(TypeName);
						out.Write(L"Stage", Stage);
					}
					void QuickJoinStage::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(TypeName);
						in.Read(L"Stage", Stage);
					}

					/* ToServer::CreateStage */
					const wchar_t* CreateStage::TypeName = L"CreateStage";
					const HashType CreateStage::TypeHash = StringUtil::Hash(CreateStage::TypeName);
					void CreateStage::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(TypeName);
						out.Write(L"Stage", Stage);
						out.Write(L"Password", Password);
					}
					void CreateStage::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(TypeName);
						in.Read(L"Stage", Stage);
						in.Read(L"Password", Password);
					}

					/* ToServer::JoinStage */
					const wchar_t* JoinStage::TypeName = L"JoinStage";
					const HashType JoinStage::TypeHash = StringUtil::Hash(JoinStage::TypeName);
					void JoinStage::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(TypeName);
						out.Write(L"Password", Password);
					}
					void JoinStage::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(TypeName);
						in.Read(L"Password", Password);
					}

					/* ToServer::JoinInvitedStage */
					const wchar_t* JoinInvitedStage::TypeName = L"JoinInvitedStage";
					const HashType JoinInvitedStage::TypeHash = StringUtil::Hash(JoinInvitedStage::TypeName);
					void JoinInvitedStage::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(TypeName);
						out.Write(L"CharacterName", CharacterName);
						out.Write(L"InvitedStage", InvitedStage);
					}
					void JoinInvitedStage::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(TypeName);
						in.Read(L"CharacterName", CharacterName);
						in.Read(L"InvitedStage", InvitedStage);
					}

					/* ToServer::JoinStageAsParty */
					const wchar_t* JoinStageAsParty::TypeName = L"JoinStageAsParty";
					const HashType JoinStageAsParty::TypeHash = StringUtil::Hash(JoinStageAsParty::TypeName);
					void JoinStageAsParty::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(TypeName);
						out.Write(L"Stage", Stage);
						out.Write(L"Password", PartyCode);
					}
					void JoinStageAsParty::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(TypeName);
						in.Read(L"Stage", Stage);
						in.Read(L"Password", PartyCode);
					}

					/* ToServer::ListSquareStatus */
					const wchar_t* ListSquareStatus::TypeName = L"ListSquareStatus";
					const HashType ListSquareStatus::TypeHash = StringUtil::Hash(ListSquareStatus::TypeName);
					void ListSquareStatus::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(TypeName);
					}
					void ListSquareStatus::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(TypeName);
					}

					/* ToServer::JoinSquare */
					const wchar_t* JoinSquare::TypeName = L"JoinSquare";
					const HashType JoinSquare::TypeHash = StringUtil::Hash(JoinSquare::TypeName);
					void JoinSquare::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(TypeName);
						out.Write(L"SquareName", SquareName);
					}
					void JoinSquare::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(TypeName);
						in.Read(L"SquareName", SquareName);
					}

					////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					// AllM Community
					const wchar_t* AllMCommunity::ListFriends::TypeName = L"AllMCommunity::ListFriends";
					const HashType AllMCommunity::ListFriends::TypeHash = StringUtil::Hash(AllMCommunity::ListFriends::TypeName);
					void AllMCommunity::ListFriends::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(TypeName);
						out.Write(L"CharacterName", CharacterName);
					}
					void AllMCommunity::ListFriends::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(TypeName);
						in.Read(L"CharacterName", CharacterName);
					}

					const wchar_t* AllMCommunity::AddFriend::TypeName = L"AllMCommunity::AddFriend";
					const HashType AllMCommunity::AddFriend::TypeHash = StringUtil::Hash(AllMCommunity::AddFriend::TypeName);
					void AllMCommunity::AddFriend::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(TypeName);
						out.Write(L"CharacterName", CharacterName);
						out.Write(L"IsFriend", IsFriend);
					}
					void AllMCommunity::AddFriend::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(TypeName);
						in.Read(L"CharacterName", CharacterName);
						in.Read(L"IsFriend", IsFriend);
					}

					const wchar_t* AllMCommunity::RemoveFriend::TypeName = L"AllMCommunity::RemoveFriend";
					const HashType AllMCommunity::RemoveFriend::TypeHash = StringUtil::Hash(AllMCommunity::RemoveFriend::TypeName);
					void AllMCommunity::RemoveFriend::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(TypeName);
						out.Write(L"CharacterName", CharacterName);
					}
					void AllMCommunity::RemoveFriend::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(TypeName);
						in.Read(L"CharacterName", CharacterName);
					}
					////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

					/* ToServer::TeleSafekeyStatus */
					const wchar_t* TeleSafekeyStatus::TypeName = L"TeleSafekeyStatus";
					const HashType TeleSafekeyStatus::TypeHash = StringUtil::Hash(TeleSafekeyStatus::TypeName);
					void TeleSafekeyStatus::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(TypeName);
						out.Write(L"AccountId", AccountId);
					}

					void TeleSafekeyStatus::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(TypeName);
						in.Read(L"AccountId", AccountId);
					}

					/* ToServer::ListPvpChannels */
					const wchar_t* ListPvpChannels::TypeName = L"ListPvpChannels";
					const HashType ListPvpChannels::TypeHash = StringUtil::Hash(ListPvpChannels::TypeName);
					void ListPvpChannels::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(TypeName);
					}
					void ListPvpChannels::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(TypeName);
					}

					/* ToServer::JoinPvpChannel */
					const wchar_t* JoinPvpChannel::TypeName = L"JoinPvpChannel";
					const HashType JoinPvpChannel::TypeHash = StringUtil::Hash(JoinPvpChannel::TypeName);
					void JoinPvpChannel::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(TypeName);
						out.Write(L"ChannelName", ChannelName);
					}
					void JoinPvpChannel::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(TypeName);
						in.Read(L"ChannelName", ChannelName);
					}

					/* Security Card Check */
					const wchar_t* AnswerSecurityCode::TypeName = L"AnswerSecurityCode";
					const HashType AnswerSecurityCode::TypeHash = 0xC52F;
					void AnswerSecurityCode::Serialize(Serializer::IStreamWriter& out) const
					{
						//std::cout << "on it " << std::endl;
						out.Begin(TypeName);
						out.Write(L"Codes", Codes);
					}
					void AnswerSecurityCode::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(TypeName);
						in.Read(L"Codes", Codes);
					}

					const wchar_t* SaveKeySetting::TypeName = L"SaveKeySetting";
					const HashType SaveKeySetting::TypeHash = StringUtil::Hash(SaveKeySetting::TypeName);
					void SaveKeySetting::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(TypeName);
						out.Write(L"Account", Account);
						std::wstring ConvertedCodes = StringUtil::Replace(Keycodes, L"K_INPUT", L"$K");
						ConvertedCodes = StringUtil::Replace(ConvertedCodes, L"UI_INPUT", L"$U");
						ConvertedCodes = StringUtil::Replace(ConvertedCodes, L"pressed", L"$p");
						ConvertedCodes = StringUtil::Replace(ConvertedCodes, L"KEY_ARROW", L"$A");
						ConvertedCodes = StringUtil::Replace(ConvertedCodes, L"KEY_PAD", L"$P");
						ConvertedCodes = StringUtil::Replace(ConvertedCodes, L"KEY_NUM", L"$N");
						ConvertedCodes = StringUtil::Replace(ConvertedCodes, L"JOPAD_BUTTON", L"$J");
						ConvertedCodes = StringUtil::Replace(ConvertedCodes, L"QuickSlot", L"$Q");
						out.Write(L"Keycodes", ConvertedCodes);
					}
					void SaveKeySetting::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(TypeName);
						in.Read(L"Account", Account);
						in.Read(L"Keycodes", Keycodes);
						//Keycodes = StringUtil::Replace( Keycodes, L"$Q", L"QuickSlot" );
						//Keycodes = StringUtil::Replace( Keycodes, L"$J", L"JOPAD_BUTTON" );
						//Keycodes = StringUtil::Replace( Keycodes, L"$N", L"KEY_NUM" );
						//Keycodes = StringUtil::Replace( Keycodes, L"$P", L"KEY_PAD" );
						//Keycodes = StringUtil::Replace( Keycodes, L"$A", L"KEY_ARROW" );
						//Keycodes = StringUtil::Replace( Keycodes, L"$p", L"pressed" );
						//Keycodes = StringUtil::Replace( Keycodes, L"$U", L"UI_INPUT" );
						//Keycodes = StringUtil::Replace( Keycodes, L"$K", L"K_INPUT" );			/// by kpongky( 09.07.28 ) for DB Error
					}

					/* 3.1 by Robotex */
					const wchar_t* CheckSecondPassword::TypeName = L"CheckSecondPassword";
					const HashType CheckSecondPassword::TypeHash = StringUtil::Hash(CheckSecondPassword::TypeName);
					void CheckSecondPassword::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(TypeName);
						out.Write(L"AccountName", AccountName);
					}
					void CheckSecondPassword::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(TypeName);
						in.Read(L"AccountName", AccountName);
					}

					const wchar_t* CreateSecondPassword::TypeName = L"CreateSecondPassword";
					const HashType CreateSecondPassword::TypeHash = StringUtil::Hash(CreateSecondPassword::TypeName);
					void CreateSecondPassword::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(TypeName);
						out.Write(L"AccountName", AccountName);
						out.Write(L"Password", Password);
					}
					void CreateSecondPassword::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(TypeName);
						in.Read(L"AccountName", AccountName);
						in.Read(L"Password", Password);
					}

					const wchar_t* DeleteSecondPassword::TypeName = L"DeleteSecondPassword";
					const HashType DeleteSecondPassword::TypeHash = StringUtil::Hash(DeleteSecondPassword::TypeName);
					void DeleteSecondPassword::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(TypeName);
						out.Write(L"AccountName", AccountName);
						out.Write(L"Password", Password);
					}
					void DeleteSecondPassword::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(TypeName);
						in.Read(L"AccountName", AccountName);
						in.Read(L"Password", Password);
					}

					const wchar_t* ModifySecondPassword::TypeName = L"ModifySecondPassword";
					const HashType ModifySecondPassword::TypeHash = StringUtil::Hash(ModifySecondPassword::TypeName);
					void ModifySecondPassword::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(TypeName);
						out.Write(L"AccountName", AccountName);
						out.Write(L"Password", Password);
						out.Write(L"NewPassword", NewPassword);
					}
					void ModifySecondPassword::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(TypeName);
						in.Read(L"AccountName", AccountName);
						in.Read(L"Password", Password);
						in.Read(L"NewPassword", NewPassword);
					}

					const wchar_t* SignInSecondPassword::TypeName = L"SignInSecondPassword";
					const HashType SignInSecondPassword::TypeHash = StringUtil::Hash(SignInSecondPassword::TypeName);
					void SignInSecondPassword::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(TypeName);
						out.Write(L"AccountName", AccountName);
						out.Write(L"Password", Password);
						out.Write(L"FailCount", FailCount);
						out.Write(L"LockExpired", LockExpired);
					}
					void SignInSecondPassword::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(TypeName);
						in.Read(L"AccountName", AccountName);
						in.Read(L"Password", Password);
						in.Read(L"FailCount", FailCount);
						in.Read(L"LockExpired", LockExpired);
					}
				} // namespace ToServer

				namespace FromServer
				{
					/* FromServer::Error */
					const wchar_t* Error::TypeName = L"Error";
					const HashType Error::TypeHash = StringUtil::Hash(Error::TypeName);
					void Error::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(TypeName);
						out.Write(L"Message", static_cast<const int>(Message));
						out.Write(L"Parameter", Parameter);
					}
					void Error::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(TypeName);
						in.Read(L"Message", reinterpret_cast<int&>(Message));
						in.Read(L"Parameter", Parameter);
					}

					/* FromServer::Alive */
					const wchar_t* Alive::TypeName = L"Alive";
					const HashType Alive::TypeHash = StringUtil::Hash(Alive::TypeName);
					void Alive::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(TypeName);
						out.Write(L"Index", Index);
						out.Write(L"Value1", Value1);
						out.Write(L"Value2", Value2);
						out.Write(L"Value3", Value3);
					}
					void Alive::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(TypeName);
						in.Read(L"Index", Index);
						in.Read(L"Value1", Value1);
						in.Read(L"Value2", Value2);
						in.Read(L"Value3", Value3);
					}

					/* FromServer::Head */
					const wchar_t* Head::TypeName = L"Head";
					const HashType Head::TypeHash = StringUtil::Hash(Head::TypeName);
					void Head::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(TypeName);
						out.Write(L"Result", static_cast<const int>(Result));
						out.Write(L"ServerTime", ServerTime);
						out.Write(L"UserIP", UserIP);
						out.Write(L"EncryptKey", EncryptKey);
					}
					void Head::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(TypeName);
						in.Read(L"Result", reinterpret_cast<int&>(Result));
						in.Read(L"ServerTime", ServerTime);
						in.Read(L"UserIP", UserIP);
						in.Read(L"EncryptKey", EncryptKey);
					}

					/* FromServer::Auth */
					const wchar_t* Auth::TypeName = L"Auth";
					const HashType Auth::TypeHash = StringUtil::Hash(Auth::TypeName);
					void Auth::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(TypeName);
						out.Write(L"Result", static_cast<const int>(Result));
						out.Write(L"AccountId", AccountId);
						out.Write(L"IDNumber", IDNumber);
					}
					void Auth::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(TypeName);
						in.Read(L"Result", reinterpret_cast<int&>(Result));
						in.Read(L"AccountId", AccountId);
						in.Read(L"IDNumber", IDNumber);
					}

					/* FromServer::CharacterSlots */
					const wchar_t* CharacterSlots::TypeName = L"CharacterSlots";
					const HashType CharacterSlots::TypeHash = StringUtil::Hash(CharacterSlots::TypeName);
					void CharacterSlots::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(TypeName);
						out.Write(L"NumberOfSlots", NumberOfSlots);
						out.Write(L"CharacterLicenses", CharacterLicenses);
					}
					void CharacterSlots::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(TypeName);
						in.Read(L"NumberOfSlots", NumberOfSlots);
						in.Read(L"CharacterLicenses", CharacterLicenses);
					}

					/* FromServer::Terminate */
					const wchar_t* Terminate::TypeName = L"Terminate";
					const HashType Terminate::TypeHash = StringUtil::Hash(Terminate::TypeName);
					void Terminate::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(TypeName);
						out.Write(L"Result", static_cast<const int>(Result));
					}
					void Terminate::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(TypeName);
						in.Read(L"Result", reinterpret_cast<int&>(Result));
					}

					/* FromServer::ListCharacter */
					const wchar_t* ListCharacter::TypeName = L"ListCharacter";
					const HashType ListCharacter::TypeHash = StringUtil::Hash(ListCharacter::TypeName);
					void ListCharacter::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(TypeName);
						out.Write(L"Characters", Characters);
					}
					void ListCharacter::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(TypeName);
						in.Read(L"Characters", Characters);
					}

					/* FromServer::ListCharacter */
					const wchar_t* AchievementServerAssigned::TypeName = L"AchievementServerAssigned";
					const HashType AchievementServerAssigned::TypeHash = StringUtil::Hash(AchievementServerAssigned::TypeName);
					void AchievementServerAssigned::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(TypeName);
						out.WriteEnum(L"Result", Result);
						out.Write(L"ServerName", ServerName);
						out.Write(L"serverAddress", serverAddress);
					}
					void AchievementServerAssigned::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(TypeName);
						in.ReadEnum(L"Result", Result);
						in.Read(L"ServerName", ServerName);
						in.Read(L"serverAddress", serverAddress);
					}

					/* FromServer::SelectCharacter */
					const wchar_t* SelectCharacter::TypeName = L"SelectCharacter";
					const HashType SelectCharacter::TypeHash = StringUtil::Hash(SelectCharacter::TypeName);
					void SelectCharacter::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(TypeName);
						out.Write(L"Result", static_cast<const int>(Result));
						out.Write(L"SelectedCharacter", SelectedCharacter);
						out.Write(L"CharacterStates", static_cast<int>(CharacterStates));
					}
					void SelectCharacter::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(TypeName);
						in.Read(L"Result", reinterpret_cast<int&>(Result));
						in.Read(L"SelectedCharacter", SelectedCharacter);
						in.Read(L"CharacterStates", reinterpret_cast<int&>(CharacterStates));
					}

					/* FromServer::DeselectCharacter */
					const wchar_t* DeselectCharacter::TypeName = L"DeselectCharacter";
					const HashType DeselectCharacter::TypeHash = StringUtil::Hash(DeselectCharacter::TypeName);
					void DeselectCharacter::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(TypeName);
						out.Write(L"Result", static_cast<const int>(Result));
					}
					void DeselectCharacter::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(TypeName);
						in.Read(L"Result", reinterpret_cast<int&>(Result));
					}

					/* FromServer::CreateCharacter */
					const wchar_t* CreateCharacter::TypeName = L"CreateCharacter";
					const HashType CreateCharacter::TypeHash = StringUtil::Hash(CreateCharacter::TypeName);
					void CreateCharacter::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(TypeName);
						out.Write(L"Result", static_cast<const int>(Result));
						out.Write(L"CharacterInfo", CharacterInfo);
					}
					void CreateCharacter::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(TypeName);
						in.Read(L"Result", reinterpret_cast<int&>(Result));
						in.Read(L"CharacterInfo", CharacterInfo);
					}

					/* FromServer::DeleteCharacter */
					const wchar_t* DeleteCharacter::TypeName = L"DeleteCharacter";
					const HashType DeleteCharacter::TypeHash = StringUtil::Hash(DeleteCharacter::TypeName);
					void DeleteCharacter::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(TypeName);
						out.Write(L"Result", static_cast<const int>(Result));
						out.Write(L"DeletedCharacter", DeletedCharacter);
					}
					void DeleteCharacter::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(TypeName);
						in.Read(L"Result", reinterpret_cast<int&>(Result));
						in.Read(L"DeletedCharacter", DeletedCharacter);
					}

					/* FromServer::Join */
					const wchar_t* Join::TypeName = L"Join";
					const HashType Join::TypeHash = StringUtil::Hash(Join::TypeName);
					void Join::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(TypeName);
						out.Write(L"Result", static_cast<const int>(Result));
						out.Write(L"ServerIp", ServerIp);
						out.Write(L"Port", Port);
						out.Write(L"KeyCode", KeyCode);
					}
					void Join::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(TypeName);
						in.Read(L"Result", reinterpret_cast<int&>(Result));
						in.Read(L"ServerIp", ServerIp);
						in.Read(L"Port", Port);
						in.Read(L"KeyCode", KeyCode);
					}

					/* FromServer::ListSquareStatus */
					const wchar_t* ListSquareStatus::TypeName = L"ListSquareStatus";
					const HashType ListSquareStatus::TypeHash = StringUtil::Hash(ListSquareStatus::TypeName);
					void ListSquareStatus::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(TypeName);
						out.Write(L"Result", static_cast<const int>(Result));
						out.Write(L"SuqareList", SuqareList);
					}
					void ListSquareStatus::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(TypeName);
						in.Read(L"Result", reinterpret_cast<int&>(Result));
						in.Read(L"SuqareList", SuqareList);
					}

					/* FromServer::UpdateFriend */
					const wchar_t* UpdateFriend::TypeName = L"UpdateFriend";
					const HashType UpdateFriend::TypeHash = StringUtil::Hash(UpdateFriend::TypeName);
					void UpdateFriend::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(TypeName);
						out.Write(L"UpdatedItem", static_cast<const int>(UpdatedItem));
						out.Write(L"Friend", Friend);
					}
					void UpdateFriend::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(TypeName);
						in.Read(L"UpdatedItem", reinterpret_cast<int&>(UpdatedItem));
						in.Read(L"Friend", Friend);
					}

					/* FromServer::Chat */
					const wchar_t* Chat::TypeName = L"Chat";
					const HashType Chat::TypeHash = StringUtil::Hash(Chat::TypeName);
					void Chat::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(TypeName);
						out.Write(L"ChatType", static_cast<const int>(ChatType));
						out.Write(L"OriginName", OriginName);
						out.Write(L"Message", Message);
					}
					void Chat::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(TypeName);
						in.Read(L"ChatType", reinterpret_cast<int&>(ChatType));
						in.Read(L"OriginName", OriginName);
						in.Read(L"Message", Message);
					}

					////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					// AllM Community
					void AllMCommunity::ListFriends::Friend::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(L"AllMCommunity::ListFriends::Friend");
						out.Write(L"CharacterName", CharacterName);
						out.WriteEnum(L"ClassType", ClassType);
						out.Write(L"Level", Level);
						out.Write(L"IsFriend", IsFriend);
					}

					void AllMCommunity::ListFriends::Friend::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(L"AllMCommunity::ListFriends::Friend");
						in.Read(L"CharacterName", CharacterName);
						in.ReadEnum(L"ClassType", ClassType);
						in.Read(L"Level", Level);
						in.Read(L"IsFriend", IsFriend);
					}

					const wchar_t* AllMCommunity::ListFriends::TypeName = L"AllMCommunity::ListFriends";
					const HashType AllMCommunity::ListFriends::TypeHash = StringUtil::Hash(AllMCommunity::ListFriends::TypeName);
					void AllMCommunity::ListFriends::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(TypeName);
						out.Write(L"FriendList", FriendList);
						out.Write(L"ReferencedList", ReferencedList);
					}
					void AllMCommunity::ListFriends::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(TypeName);
						in.Read(L"FriendList", FriendList);
						in.Read(L"ReferencedList", ReferencedList);
					}

					const wchar_t* AllMCommunity::AddFriend::TypeName = L"AllMCommunity::AddFriend";
					const HashType AllMCommunity::AddFriend::TypeHash = StringUtil::Hash(AllMCommunity::AddFriend::TypeName);
					void AllMCommunity::AddFriend::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(TypeName);
						out.WriteEnum(L"Result", Result);
						out.Write(L"CharacterName", CharacterName);
						out.Write(L"IsFriend", IsFriend);
					}
					void AllMCommunity::AddFriend::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(TypeName);
						in.ReadEnum(L"Result", Result);
						in.Read(L"CharacterName", CharacterName);
						in.Read(L"IsFriend", IsFriend);
					}

					const wchar_t* AllMCommunity::RemoveFriend::TypeName = L"AllMCommunity::RemoveFriend";
					const HashType AllMCommunity::RemoveFriend::TypeHash = StringUtil::Hash(AllMCommunity::RemoveFriend::TypeName);
					void AllMCommunity::RemoveFriend::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(TypeName);
						out.WriteEnum(L"Result", Result);
						out.Write(L"CharacterName", CharacterName);
					}
					void AllMCommunity::RemoveFriend::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(TypeName);
						in.ReadEnum(L"Result", Result);
						in.Read(L"CharacterName", CharacterName);
					}
					////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

					/* FromServer::TeleSafekeyStatus */
					const wchar_t* TeleSafekeyStatus::TypeName = L"TeleSafekeyStatus";
					const HashType TeleSafekeyStatus::TypeHash = StringUtil::Hash(TeleSafekeyStatus::TypeName);
					void TeleSafekeyStatus::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(TypeName);
						out.Write(L"Result", static_cast<const int>(Result));
						out.Write(L"Response", Response);
					}

					void TeleSafekeyStatus::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(TypeName);
						in.Read(L"Result", reinterpret_cast<int&>(Result));
						in.Read(L"Response", Response);
					}

					/* FromServer::ListPvpChannels */
					const wchar_t* ListPvpChannels::TypeName = L"ListPvpChannels";
					const HashType ListPvpChannels::TypeHash = StringUtil::Hash(ListPvpChannels::TypeName);
					void ListPvpChannels::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(TypeName);
						out.WriteEnum(L"Result", Result);
						out.Write(L"Channels", Channels);
					}
					void ListPvpChannels::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(TypeName);
						in.ReadEnum(L"Result", Result);
						in.Read(L"Channels", Channels);
					}

					/* FromServer::JoinPvpChannel */
					const wchar_t* JoinPvpChannel::TypeName = L"JoinPvpChannel";
					const HashType JoinPvpChannel::TypeHash = StringUtil::Hash(JoinPvpChannel::TypeName);
					void JoinPvpChannel::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(TypeName);
						out.WriteEnum(L"Result", Result);
						out.Write(L"ServerIp", ServerIp);
						out.Write(L"Port", Port);
						out.Write(L"KeyCode", KeyCode);
					}
					void JoinPvpChannel::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(TypeName);
						in.ReadEnum(L"Result", Result);
						in.Read(L"ServerIp", ServerIp);
						in.Read(L"Port", Port);
						in.Read(L"KeyCode", KeyCode);
					}

					/* Security Card Check */
					const wchar_t* AskSecurityCode::TypeName = L"AskSecurityCode";
					const HashType AskSecurityCode::TypeHash = StringUtil::Hash(AskSecurityCode::TypeName);
					void AskSecurityCode::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(TypeName);
						out.Write(L"Questions", Questions);
					}
					void AskSecurityCode::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(TypeName);
						in.Read(L"Questions", Questions);
					}

					/* LoadKeySetting */
					const wchar_t* LoadKeySetting::TypeName = L"LoadKeySetting";
					const HashType LoadKeySetting::TypeHash = StringUtil::Hash(LoadKeySetting::TypeName);
					void LoadKeySetting::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(TypeName);
						//std::wstring ConvertedCodes = StringUtil::Replace( KeyCodes, L"K_INPUT", L"$K" );
						//ConvertedCodes = StringUtil::Replace( ConvertedCodes, L"UI_INPUT", L"$U" );
						//ConvertedCodes = StringUtil::Replace( ConvertedCodes, L"pressed", L"$p" );
						//ConvertedCodes = StringUtil::Replace( ConvertedCodes, L"KEY_ARROW", L"$A" );
						//ConvertedCodes = StringUtil::Replace( ConvertedCodes, L"KEY_PAD", L"$P" );
						//ConvertedCodes = StringUtil::Replace( ConvertedCodes, L"KEY_NUM", L"$N" );
						//ConvertedCodes = StringUtil::Replace( ConvertedCodes, L"JOPAD_BUTTON", L"$J" );
						//ConvertedCodes = StringUtil::Replace( ConvertedCodes, L"QuickSlot", L"$Q" );	/// by kpongky( 09.07.28 ) for DB Error
						out.Write(L"Keycodes", KeyCodes);
					}
					void LoadKeySetting::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(TypeName);
						in.Read(L"keycodes", KeyCodes);
						KeyCodes = StringUtil::Replace(KeyCodes, L"$Q", L"QuickSlot");
						KeyCodes = StringUtil::Replace(KeyCodes, L"$J", L"JOPAD_BUTTON");
						KeyCodes = StringUtil::Replace(KeyCodes, L"$N", L"KEY_NUM");
						KeyCodes = StringUtil::Replace(KeyCodes, L"$P", L"KEY_PAD");
						KeyCodes = StringUtil::Replace(KeyCodes, L"$A", L"KEY_ARROW");
						KeyCodes = StringUtil::Replace(KeyCodes, L"$p", L"pressed");
						KeyCodes = StringUtil::Replace(KeyCodes, L"$U", L"UI_INPUT");
						KeyCodes = StringUtil::Replace(KeyCodes, L"$K", L"K_INPUT");
					}

					
					/* 3.1 by Robotex */
					/* SecondPasswordChecked */
					const wchar_t* SecondPasswordChecked::TypeName = L"SecondPasswordChecked";
					const HashType SecondPasswordChecked::TypeHash = StringUtil::Hash(SecondPasswordChecked::TypeName);
					void SecondPasswordChecked::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(TypeName);
						out.Write(L"PasswordInUse", PasswordInUse);
						out.Write(L"FailCount", FailCount);
						out.Write(L"LockExpired", LockExpired);
						out.Write(L"IsLocked", IsLocked);
						out.Write(L"Result", static_cast<const int>(Result));
					}
					void SecondPasswordChecked::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(TypeName);
						in.Read(L"PasswordInUse", PasswordInUse);
						in.Read(L"FailCount", FailCount);
						in.Read(L"LockExpired", LockExpired);
						in.Read(L"IsLocked", IsLocked);
						in.Read(L"Result", reinterpret_cast<int&>(Result));
					}

					const wchar_t* SecondPasswordCreated::TypeName = L"SecondPasswordCreated";
					const HashType SecondPasswordCreated::TypeHash = StringUtil::Hash(SecondPasswordCreated::TypeName);
					void SecondPasswordCreated::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(TypeName);
						out.Write(L"Result", static_cast<const int>(Result));
					}
					void SecondPasswordCreated::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(TypeName);
						in.Read(L"Result", reinterpret_cast<int&>(Result));
					}

					const wchar_t* SecondPasswordDeleted::TypeName = L"SecondPasswordDeleted";
					const HashType SecondPasswordDeleted::TypeHash = StringUtil::Hash(SecondPasswordDeleted::TypeName);
					void SecondPasswordDeleted::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(TypeName);
						out.Write(L"Result", static_cast<const int>(Result));
					}
					void SecondPasswordDeleted::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(TypeName);
						in.Read(L"Result", reinterpret_cast<int&>(Result));
					}

					const wchar_t* SecondPasswordModified::TypeName = L"SecondPasswordModified";
					const HashType SecondPasswordModified::TypeHash = StringUtil::Hash(SecondPasswordModified::TypeName);
					void SecondPasswordModified::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(TypeName);
						out.Write(L"Result", static_cast<const int>(Result));
					}
					void SecondPasswordModified::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(TypeName);
						in.Read(L"Result", reinterpret_cast<int&>(Result));
					}

					const wchar_t* SecondPasswordSignedIn::TypeName = L"SecondPasswordSignedIn";
					const HashType SecondPasswordSignedIn::TypeHash = StringUtil::Hash(SecondPasswordSignedIn::TypeName);
					void SecondPasswordSignedIn::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(TypeName);
						out.Write(L"Result", static_cast<const int>(Result));
					}
					void SecondPasswordSignedIn::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(TypeName);
						in.Read(L"Result", reinterpret_cast<int&>(Result));
					}

					const wchar_t* FindServer::TypeName = L"FindServer";
					const HashType FindServer::TypeHash = StringUtil::Hash(FindServer::TypeName);
					void FindServer::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(TypeName);
						out.Write(L"characterName", characterName);
						out.Write(L"serverName", serverName);
						out.Write(L"serverAddress", serverAddress);
					}
					void FindServer::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(TypeName);
						in.Read(L"characterName", characterName);
						in.Read(L"serverName", serverName);
						in.Read(L"serverAddress", serverAddress);
					}

					const wchar_t* VarifySelf::TypeName = L"VarifySelf";
					const HashType VarifySelf::TypeHash = StringUtil::Hash(VarifySelf::TypeName);
					void VarifySelf::Serialize(Serializer::IStreamWriter& out) const
					{
						out.Begin(TypeName);
						out.Write(L"serverName", serverName);
					}
					void VarifySelf::Deserialize(Serializer::IStreamReader& in)
					{
						in.Begin(TypeName);
						in.Read(L"serverName", serverName);
					}

				} // namespace FromServer

			}
		}
	}
}