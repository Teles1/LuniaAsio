#include "StageServerProtocol.h"
namespace Lunia {
	namespace StageServer {
		namespace Protocol {
            const wchar_t* Error::TypeName = L"Error";
            const HashType Error::TypeHash = StringUtil::Hash(Error::TypeName);
            void Error::Serialize(Serializer::IStreamWriter& out) const {
                out.Begin(TypeName);
                out.Write(L"errorcode", errorcode);
                out.Write(L"errorstring", errorstring);
            }
            void Error::Deserialize(Serializer::IStreamReader& in) {
                in.Begin(TypeName);
                in.Read(L"errorcode", errorcode);
                in.Read(L"errorstring", errorstring);
            }

            const wchar_t* Way::TypeName = L"Way";
            const HashType Way::TypeHash = StringUtil::Hash(Way::TypeName);
            void Way::Serialize(Serializer::IStreamWriter& out) const {
                out.Begin(TypeName);
                out.Write(L"EncryptKey", EncryptKey);
            }
            void Way::Deserialize(Serializer::IStreamReader& in) {
                in.Begin(TypeName);
                in.Read(L"EncryptKey", EncryptKey);
            }

            const wchar_t* Alive::TypeName = L"Alive";
            const HashType Alive::TypeHash = StringUtil::Hash(Alive::TypeName);
            void Alive::Serialize(Serializer::IStreamWriter& out) const {
                out.Begin(TypeName);
                out.Write(L"EncryptKey", EncryptKey);
                out.Write(L"RequestTime", RequestTime);
            }
            void Alive::Deserialize(Serializer::IStreamReader& in) {
                in.Begin(TypeName);
                in.Read(L"EncryptKey", EncryptKey);
                in.Read(L"RequestTime", RequestTime);
            }

            const wchar_t* Stage::TypeName = L"Stage";
            const HashType Stage::TypeHash = StringUtil::Hash(Stage::TypeName);
            void Stage::Serialize(Serializer::IStreamWriter& out) const {
                out.Begin(TypeName);
                out.Write(L"charactername", charactername);
                out.Write(L"targetStage", targetStage);
            }
            void Stage::Deserialize(Serializer::IStreamReader& in) {
                in.Begin(TypeName);
                in.Read(L"secukey", secukey);
                in.Read(L"Version", Version);
                in.Read(L"Locale", Locale);
            }
		}
	}
}