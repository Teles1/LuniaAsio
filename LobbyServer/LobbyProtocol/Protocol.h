#pragma once
#include "../../Core/Common.h"
#include "../../Core/Serializer/Serializer.h"
#ifndef NET_SERIALIZABLE
#define NET_SERIALIZABLE								    \
    static String TypeName;									\
    static const uint16 TypeHash;							\
    void Serialize(Serializer::StreamWriter &out) const;	\
    void Deserialize(Serializer::StreamReader &in);			\
    const String GetTypeName() { return TypeName; }
#endif
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

		static const String TypeName;
		static const uint16 TypeHash;
		void Serialize(Serializer::StreamWriter& out) const;
		void Deserialize(Serializer::StreamReader& in);
		const String GetTypeName() { return TypeName; }
	};
}//ToServer
}//Protocol
}//Lobby
