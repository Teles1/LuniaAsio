#pragma once
#include "../../Core/Serializer/StreamReader.h"
namespace Network {
	const HashType NetStreamHash = StringUtil::Hash(L"NetStream");

	int GetNetStreamHeaderSize() {
		return sizeof(HashType) + sizeof(LengthType);
	}

	class NetStream : private Serializer::StreamReader {
	private:
	public:
		NetStream(uint8* buffer, size_t length)
			: StreamReader(buffer, length) {}
		int GetNetStreamSize() {
			const LengthType* unit = (const LengthType*)buffer;
			return *(unit);
		}
		bool IsNetStream() {
			char* check = (char*)buffer + sizeof(LengthType);
			return *(HashType*)check == NetStreamHash ? true : false;
		}
		HashType GetSerializedTypeHash() {
			HashType* temp = (HashType*)(buffer + GetNetStreamHeaderSize());
			return *temp;
		}
	};
}
