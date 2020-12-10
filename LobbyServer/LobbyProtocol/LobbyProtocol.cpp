#pragma once
#include "LobbyProtocol.h"
namespace Lunia {
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
				Logger::GetInstance()->Warn("ListCharacter shouldn't be getting called by the client.");
			}


			const wchar_t* CharacterSlots::TypeName = L"CharacterSlots";
			const uint16 CharacterSlots::TypeHash = StringUtil::Hash(TypeName);
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
		}
	}
}