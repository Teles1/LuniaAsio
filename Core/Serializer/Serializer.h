#pragma once
#include "../Common.h"
#include "../Utils/Version.h"

namespace Serializer {


	class StreamWriter
	{
	private:
		uint8* buffer;
		uint16 position;
	public:
		StreamWriter() {
			buffer = new uint8[MAX_BUFFER_SIZE];
			position = 0;
		}
		~StreamWriter() {
			delete this->buffer;
			this->position = 0;
		}
	};
	class StreamReader
	{
	public:
		StreamReader(uint8* inData, uint16 size){
			this->buffer = new uint8[size];
			memcpy(this->buffer, inData, size);
			this->size = size;
			this->position = 0;
		}
		StreamReader(std::vector<uint8> inData){
			this->size = inData.size();
			this->buffer = new uint8[this->size];
			memcpy(this->buffer, &inData[0], this->size);
			this->position = 0;
		}
		StreamReader() {
			this->buffer = new uint8[0];
			this->size = 0;
			this->position = 0;
		}
		uint16 GetData(uint8*& buffer) {
			buffer = new uint8[this->size - this->position];
			memcpy(buffer, this->buffer, this->size - this->position);
			return this->position;
		}
		template<typename T> 
		void Read(T& value) {
			CanRead(sizeof(T));
			memcpy(&value, this->buffer + this->position, sizeof(T));
			this->position += sizeof(T);
		}
		~StreamReader() 
		{
			delete this->buffer;
			this->position = 0;
			this->size = 0;
		}
	protected:
		uint8* buffer;
		uint16 position;
		uint16 size;
		void CanRead(uint16 lenght) {
			if (this->position + lenght > size) {
				throw Exception(boost::format("Cannot read past buffer MaxLenght(%d) Position(%d) Lenght(%d)\n") % this->size % this->position % lenght);
			}
		}
	};
}