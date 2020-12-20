#ifndef Protocol_H
#define Protocol_H
#pragma once
#include <Core/Serializer/Serializer.h>

namespace Lunia {
	namespace Protocol {
		struct Error : public Serializer::ISerializable
		{
			uint16		ErrorCode;
			String		Description;

			static const wchar_t* TypeName;
			void Serialize(Serializer::IStreamWriter& out) const;
			void Deserialize(Serializer::IStreamReader& in);
		};

		struct Alive : public Serializer::ISerializable
		{
			static const wchar_t* TypeName;
			void Serialize(Serializer::IStreamWriter& out) const;
			void Deserialize(Serializer::IStreamReader& in);
		};

		struct Ping : public Serializer::ISerializable
		{
			uint32		Timestamp;

			static const wchar_t* TypeName;
			void Serialize(Serializer::IStreamWriter& out) const;
			void Deserialize(Serializer::IStreamReader& in);
		};
	}
}
#endif // ! Protocol_H