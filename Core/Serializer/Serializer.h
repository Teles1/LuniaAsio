#pragma once
#include "../Common.h"
#include "../Utils/Version.h"
#include <cassert>

namespace Serializer {


	class StreamWriter
	{
	private:
		const char* buffer; //this won't ever change. The buffer will remain intact.
		uint16 position;
	public:
		StreamWriter() {
			buffer = new char[MAX_BUFFER_SIZE];
			position = 0;
		}
	};
	class StreamReader
	{
	private:
		uint8* buffer;
		uint16 position;
		uint16 size;
		void CanRead(uint16 lenght) {
			if (this->position + lenght > size) {
				throw Exception(boost::format("Cannot read past buffer MaxLenght(%d) Position(%d) Lenght(%d)\n") % this->size % this->position % lenght);
			}
		}
		void CleanUp(size_t size) {
			buffer += size;
			position += size;
		}
	public:
		StreamReader(uint8* inData, uint16 size){
			this->buffer = inData;
			this->position = 0;
			this->size = size;
		}
		StreamReader() {
			this->buffer = new uint8[0];
			this->position = 0;
			this->size = 0;
		}
		template<typename T> void Read(T& value) {
			CanRead(sizeof(value));
			memcpy(&value, buffer, sizeof(value));
			CleanUp(sizeof(value));
		}
	};

	class ISerializable {
	public:
		std::wstring TypeName = L"";
		uint16 TypeHash = 0;
		ISerializable(std::wstring input) {
			this->TypeName = input.c_str();
			this->TypeHash = StringUtil::Hash(input);
		}
		virtual void Serialize(StreamWriter& out) const = 0;
		virtual void Deserialize(StreamReader& in) = 0;
		virtual ~ISerializable() {}
	};
}