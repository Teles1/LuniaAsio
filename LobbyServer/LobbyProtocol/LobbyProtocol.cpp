#pragma once
#include "LobbyProtocol.h"
namespace Lunia {
	namespace XRated {
		namespace Lobby {
			namespace Protocol {

				const wchar_t* Head::TypeName = L"Head";
				const uint16 Head::TypeHash = StringUtil::Hash(TypeName);
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
					in.Read(L"ProtocolVersion", ProtocolVersion);
					in.Read(L"Checksums", Checksums);
				}

				const wchar_t* Auth::TypeName = L"Auth";
				const uint16 Auth::TypeHash = StringUtil::Hash(TypeName);
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
					in.Read(L"AccountId", AccountId);
					in.Read(L"EncryptedPassword", EncryptedPassword);
					in.Read(L"AuthString", AuthString, 2048);
					in.Read(L"IsPubModuleLogin", IsPubModuleLogin);
					in.Read(L"Locale", Locale);
				}

				const wchar_t* Alive::TypeName = L"Alive";
				const uint16 Alive::TypeHash = StringUtil::Hash(TypeName);
				void Alive::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(TypeName);
					out.Write(L"Index", Index);
					out.Write(L"Index", Value1);
					out.Write(L"Index", Value2);
					out.Write(L"Index", Value3);
				}
				void Alive::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(TypeName);
					in.Read(L"Index", Index);
					in.Read(L"Value1", Value1);
					in.Read(L"Value2", Value2);
					in.Read(L"Value3", Value3);
				}

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
					Logger::GetInstance().Warn("ListCharacter shouldn't be getting called by the client.");
				}

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
					in.Read(L"Name", Name);
					in.Read(L"ClassType", reinterpret_cast<int&>(ClassType));
				}

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
				}

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
					in.Read(L"Name", Name);
				}

				const wchar_t* SaveKeySetting::TypeName = L"SaveKeySetting";
				const HashType SaveKeySetting::TypeHash = StringUtil::Hash(SaveKeySetting::TypeName);
				void SaveKeySetting::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(TypeName);
				}
				void SaveKeySetting::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(TypeName);
					in.Read(L"Account", Account);
					in.Read(L"Keycodes", Keycodes);
				}

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
					in.Read(L"Name", CharacterName);
				}

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
				}

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
				}

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
			}
		}
	}
}