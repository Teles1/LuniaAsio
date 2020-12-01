#include "LobbyProtocol.h"
namespace Lobby {
	namespace Protocol {
		namespace ToServer {

			const String Head::TypeName = L"Head";
			const uint16 Head::TypeHash = StringUtil::Hash(Head::TypeName);
			void Head::Serialize(Serializer::StreamWriter& out) const
			{
				//out.Write(ProtocolVersion);
				//out.Write(Checksums);
			}
			void Head::Deserialize(Serializer::StreamReader& in)
			{
				in.Read(ProtocolVersion);
				in.Read(Checksums);
			}
			const String Auth::TypeName = L"Auth";
			const uint16 Auth::TypeHash = StringUtil::Hash(Auth::TypeName);
			void Auth::Serialize(Serializer::StreamWriter& out) const
			{
				//out.Write(ProtocolVersion);
				//out.Write(Checksums);
			}
			void Auth::Deserialize(Serializer::StreamReader& in)
			{
				in.Read(AccountId);
				in.Read(EncryptedPassword);
				in.Read(AuthString);
				in.Read(IsPubModuleLogin);
				in.Read(Locale);
			}

		}
	}
}
