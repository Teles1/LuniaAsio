#pragma once
#include "../Core/Serializer/Serializer.h"
namespace Network {
	class NetStream : public Serializer::ISerializable{
		Serializer::StreamReader reader;
	public:
		NetStream(uint8* stream, uint16 size) : ISerializable(L"NetStream")
		{
			this->reader = Serializer::StreamReader(stream, size);
		}

		// Inherited via ISerializable
		virtual void Serialize(Serializer::StreamWriter& out) const override;
		virtual void Deserialize(Serializer::StreamReader& in) override;
	};
}