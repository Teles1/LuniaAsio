#include "../Core/Common.h"
#include "./NetStream.h"
#include <map>
namespace Network {
	int NetStream::GetNetStreamSize() {
		if (this->size < sizeof(uint16)) throw Exception("Cannot read beyond stream. Not StreamHash");
		uint16 aux = 0;
		memcpy(&aux, this->buffer, sizeof(uint16));
		return aux;
	}

	bool NetStream::IsNetStream() {
 		if (this->GetNetStreamSize() < (sizeof(uint16) + sizeof(uint16))) // HashType + LenghtType
			return false;
		if (this->NetStreamHash == (this->buffer[2] + (this->buffer[3] << 8))) {
			this->position += 4;
			return true;
		}
		return false;
	}
	Serializer::StreamReader NetStream::GetStream() {
		uint8* buffer;
		uint16 size = this->GetData(buffer);
		return Serializer::StreamReader(buffer, size);
	}
}