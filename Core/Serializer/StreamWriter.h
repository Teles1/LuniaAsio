#pragma once
#include "../Common.h"
namespace Serializer {
	class StreamWriter
	{
	private:
	public:
		uint8* buffer;
		uint16 position;

		StreamWriter() {
			this->buffer = new uint8[MAX_BUFFER_SIZE];
			this->position = 0;
		}
		~StreamWriter() {
			delete this->buffer;
			this->position = 0;
		}
#pragma region Base Types
		void Write(uint8 value) {
			CanWrite(sizeof(uint8));
			memcpy(this->buffer + this->position, &value, sizeof(uint8));
			this->position += sizeof(uint8);
		}
		void Write(uint16 value) {
			CanWrite(sizeof(uint16));
			memcpy(this->buffer + this->position, &value, sizeof(uint16));
			this->position += sizeof(uint16);
		}
		void Write(uint32 value) {
			CanWrite(sizeof(uint32));
			memcpy(this->buffer + this->position, &value, sizeof(uint32));
			this->position += sizeof(uint32);
		}
		void Write(uint64 value) {
			CanWrite(sizeof(uint64));
			memcpy(this->buffer + this->position, &value, sizeof(uint64));
			this->position += sizeof(uint64);
		}
		void Write(int8 value) {
			CanWrite(sizeof(int8));
			memcpy(this->buffer + this->position, &value, sizeof(int8));
			this->position += sizeof(int8);
		}
		void Write(int16 value) {
			CanWrite(sizeof(int16));
			memcpy(this->buffer + this->position, &value, sizeof(int16));
			this->position += sizeof(int16);
		}
		void Write(int32 value) {
			CanWrite(sizeof(int32));
			memcpy(this->buffer + this->position, &value, sizeof(int32));
			this->position += sizeof(int32);
		}
		void Write(int64 value) {
			CanWrite(sizeof(int64));
			memcpy(this->buffer + this->position, &value, sizeof(int64));
			this->position += sizeof(int64);
		}
#pragma endregion
		void Write(std::string value) {
			CanWrite(value.size());
			uint16 size = value.size();
			Write(size);
			memcpy(this->buffer + this->position, value.c_str(), value.size());
			this->position += value.size();
		}
		template<typename T>
		void WriteContainer(T& container) {
			unsigned int size = container.size();
			Write(size);
			for (int i = 0; i < size; i++) {
				std::cout << container[i] << std::endl;
			}
		}
		template<typename T>
		void WriteKeyedContainer(T& container) {
			unsigned int size = container.size();
			Write(size);
			int counter = 0;
			for (T it : container) {
				Write(it->first);
				Write(it->second);
			}
		}

		template<typename T>
		void Write(const std::vector<T>& value) { CanWrite(sizeof(T)); WriteContainer(value); }
		template<typename T>
		void Write(std::list<T>& value) { CanWrite(sizeof(T)); WriteContainer(value); }
		template<typename T>
		void Write(std::deque<T>& value) { CanWrite(sizeof(T)); WriteContainer(value); }
		template<typename T>
		void Write(std::set<T>& value) { CanWrite(sizeof(T)); WriteContainer(value); }
		template<typename T1, typename T2>
		void Write(std::set<T1, T2>& value) {
			CanWrite(sizeof(T1));
			CanWrite(sizeof(T2));
			WriteContainer(value);
		}
		template<typename T1, typename T2>
		void Write(std::map<T1, T2>& value) {
			CanWrite(sizeof(T1));
			CanWrite(sizeof(T2));
			WriteKeyedContainer(value);
		}
		template<typename T1, typename T2, typename T3>
		void Write(std::map<T1, T2, T3>& value) {
			CanWrite(sizeof(T1));
			CanWrite(sizeof(T2));
			CanWrite(sizeof(T3));
			WriteKeyedContainer(value);
		}


	protected:
		void CanWrite(uint16 lenght) {
			if (this->position + lenght > MAX_BUFFER_SIZE) {
				throw Exception(boost::format("Cannot write past buffer MaxLenght(%d) Position(%d) Lenght(%d)\n") % MAX_BUFFER_SIZE % this->position % lenght);
			}
		}

	};
}
