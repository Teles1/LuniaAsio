#include "Protocol.h"
namespace Lunia {
	namespace Protocol {

		const wchar_t* Error::TypeName = L"Error";
		void Error::Serialize(Serializer::IStreamWriter& out) const
		{
			out.Begin(TypeName);
			out.Write(L"ErrorCode", ErrorCode);
			out.Write(L"Description", Description);
		}
		void Error::Deserialize(Serializer::IStreamReader& in)
		{
			in.Begin(TypeName);
			in.Read(L"ErrorCode", ErrorCode);
			in.Read(L"Description", Description);
		}

		const wchar_t* Alive::TypeName = L"Alive";
		void Alive::Serialize(Serializer::IStreamWriter& out) const
		{
			out.Begin(TypeName);
		}
		void Alive::Deserialize(Serializer::IStreamReader& in)
		{
			in.Begin(TypeName);
		}

		const wchar_t* Ping::TypeName = L"Ping";
		void Ping::Serialize(Serializer::IStreamWriter& out) const
		{
			out.Begin(TypeName);
			out.Write(L"Timestamp", Timestamp);
		}
		void Ping::Deserialize(Serializer::IStreamReader& in)
		{
			in.Begin(TypeName);
			in.Read(L"Timestamp", Timestamp);
		}
	}
}