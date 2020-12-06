#pragma once
#include "LobbyProtocol.h"

namespace Lobby {
	namespace Protocol {

		const uint16 Head::TypeHash = StringUtil::Hash(L"Head");
		void Head::Serialize(Serializer::StreamWriter& out)
		{
			out.Begin(TypeHash);
			out.Write(Result);
			out.Write(L"ServerTime", ServerTime);
			out.Write(UserIP);
			out.Write(EncryptKey);
		}
		void Head::Deserialize(Serializer::StreamReader& in)
		{
			in.Read(ProtocolVersion);
			in.Read(Hashcode);
			in.Read(Checksums);
		}

		const uint16 Auth::TypeHash = StringUtil::Hash(L"Auth");
		void Auth::Serialize(Serializer::StreamWriter& out)
		{
			out.Begin(TypeHash);
			out.Write(static_cast<const int>(Result));
			out.Write(AccountId);
			out.Write(IDNumber);
		}
		void Auth::Deserialize(Serializer::StreamReader& in)
		{
			in.Read(AccountId);
			in.Read(EncryptedPassword);
			in.Read(AuthString);
			in.Read(IsPubModuleLogin);
			in.Read(Locale);
		}

		const uint16 Alive::TypeHash = StringUtil::Hash(L"Alive");
		void Alive::Serialize(Serializer::StreamWriter& out)
		{
			out.Begin(TypeHash);
			out.Write(Index);
			out.Write(Value1);
			out.Write(Value2);
			out.Write(Value3);
		}
		void Alive::Deserialize(Serializer::StreamReader& in)
		{
			in.Read(Index);
			in.Read(Value1);
			in.Read(Value2);
			in.Read(Value3);
		}

		const uint16 CharacterSlots::TypeHash = StringUtil::Hash(L"CharacterSlots");
		void CharacterSlots::Serialize(Serializer::StreamWriter& out)
		{
			out.Begin(TypeHash);
			//out.Write(NumberOfSlots);
			//out.Write(CharacterLicenses);
		}
		void CharacterSlots::Deserialize(Serializer::StreamReader& in)
		{
			in.Read(NumberOfSlots);
			in.Read(CharacterLicenses);
		}
	}
}
