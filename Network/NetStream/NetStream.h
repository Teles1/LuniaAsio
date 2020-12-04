#pragma once
#include "../../Core/Serializer/Serializer.h"
namespace Network {
	int GetNetStreamHeaderSize() {
		return sizeof(HashType) + sizeof(LengthType);
	}

	class NetStreamReader : private Serializer::StreamReader {
	public:
		NetStreamReader(uint8* buffer, size_t length)
			: StreamReader(buffer, length) {}
		int GetNetStreamSize() {
			const LengthType* unit = (const LengthType*)buffer;
			return *(unit);
		}
		bool IsNetStream() {
			char* check = (char*)buffer + sizeof(LengthType);
			return *(HashType*)check == Serializer::NetStreamHash ? true : false;
		}
		HashType GetSerializedTypeHash() {
			HashType* temp = (HashType*)(buffer + GetNetStreamHeaderSize());
			return *temp;
		}
	private:
	};
}
