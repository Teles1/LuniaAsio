#pragma once
#include "../Common.h"
#include "../Core.h"
namespace Serializer {
	class StreamReader
	{
	private:
		enum {
			MaxString = 1024
		};
	public:
#pragma region Constructors
		StreamReader(uint8* inData, uint16 size) {
			this->buffer = new uint8[size];
			memcpy(this->buffer, inData, size);
			this->size = size;
			this->position = 0;
		}
		StreamReader(std::vector<uint8> inData) {
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
			memcpy(buffer, this->buffer + this->position, this->size - this->position);
			return this->position;
		}
#pragma endregion
#pragma region SimpleTypes
		void Read(uint8& value) {
			CanRead(sizeof(uint8));
			memcpy(&value, this->buffer + this->position, sizeof(uint8));
			this->position += sizeof(uint8);
		}
		void Read(uint16& value) {
			CanRead(sizeof(uint16));
			memcpy(&value, this->buffer + this->position, sizeof(uint16));
			this->position += sizeof(uint16);
		}
		void Read(uint32& value) {
			CanRead(sizeof(uint32));
			memcpy(&value, this->buffer + this->position, sizeof(uint32));
			this->position += sizeof(uint32);
		}
		void Read(uint64& value) {
			CanRead(sizeof(uint64));
			memcpy(&value, this->buffer + this->position, sizeof(uint64));
			this->position += sizeof(uint64);
		}

		void Read(int8& value) {
			CanRead(sizeof(int8));
			memcpy(&value, this->buffer + this->position, sizeof(int8));
			this->position += sizeof(int8);
		}
		void Read(int16& value) {
			CanRead(sizeof(int16));
			memcpy(&value, this->buffer + this->position, sizeof(int16));
			this->position += sizeof(int16);
		}
		void Read(int32& value) {
			CanRead(sizeof(int32));
			memcpy(&value, this->buffer + this->position, sizeof(int32));
			this->position += sizeof(int32);
		}
		void Read(int64& value) {
			CanRead(sizeof(int64));
			memcpy(&value, this->buffer + this->position, sizeof(int64));
			this->position += sizeof(int64);
		}
#pragma endregion

		void Read(std::string& value) {
			uint16 len = 0;
			Read(len);
			// now that we have the lenght we can read the chars inside.
			CanRead(len);
			value.append(reinterpret_cast<char*>(buffer + position), len);
			position += len;
		}
		void Read(String& value) {
			uint16 len(0);
			Read(len);
			CanRead(len*2); //wide charater.
			value.append(reinterpret_cast<wchar_t*>(buffer + position), len);
			position += len * 2;
		}


		void Read(std::vector<std::string>& value) {
			int vectorLenght = 0;
			Read(vectorLenght);
			value.resize(vectorLenght);
			std::string aux{};
			for (int i = 0; i < vectorLenght; i++) {
				Read(aux);
				value[i] = aux;
			}
		}
		/*
		uint32 Read(wchar_t* buffer, uint16 size) {
			String str(L"");
			ReadString(str);
			memcpy(buffer, str.c_str(), size - 1);
			return uint32(str.size());
		}
		uint32 Read(char* buffer, uint16 size) {
			std::string str("");
			ReadString(str);
			memcpy(buffer, str.c_str(), size - 1);
			return uint32(str.size());
		}
		*/
		template<typename T>
		void Read(std::vector<T>& value) {
			int vectorLenght = 0;
			Read(vectorLenght);
			T aux{};
			for (int i = 0; i < vectorLenght; i++) {
				Read(aux);
				value[i] = aux;
			}
		}
		uint16 GetPosition() {
			return this->position;
		}
		uint16 GetLenght() {
			return this->size;
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
			if (this->position + lenght > this->size) {
				throw Exception(boost::format("Cannot read past buffer MaxLenght(%d) Position(%d) Lenght(%d)\n") % this->size % this->position % lenght);
			}
		}
	};
}