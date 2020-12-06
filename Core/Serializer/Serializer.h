/*
	Header containing all includes needed for Serializer's
*/
#ifndef Serializer_HEADER_GUARD
#define Serializer_HEADER_GUARD
#include "../Core.h"
#include "../Utils/Buffer.h"
#include "ISerializable.h"
namespace Serializer {
	const HashType NetStreamHash = StringUtil::Hash(L"NetStream");
	const uint16 HeaderSize = sizeof(HashType) + sizeof(LengthType);
	class StreamReader
	{
	private:
		enum {
			MaxString = 1024
		};
	public:
#pragma region Constructors
		StreamReader(const uint8* inData, size_t size)
			: buffer(inData), endOfBuffer(inData + GetNetStreamSize()) {}
		StreamReader(std::vector<uint8> inData)
			: buffer(&inData[0]), endOfBuffer(&inData[0] + GetNetStreamSize()) {}
#pragma endregion
#pragma region SimpleTypes
		void Read(bool& value) { ReadBaseType(value); }
		void Read(uint8& value) { ReadBaseType(value); }
		void Read(uint16& value) { ReadBaseType(value); }
		void Read(uint32& value) { ReadBaseType(value); }
		void Read(uint64& value) { ReadBaseType(value); }

		void Read(int8& value) { ReadBaseType(value); }
		void Read(int16& value) { ReadBaseType(value); }
		void Read(int32& value) { ReadBaseType(value); }
		void Read(int64& value) { ReadBaseType(value); }

		void Read(ISerializable& value)
		{
			//buffer += sizeof(LengthType); // skip length information of NetStream

			// check hash
			//assert(reinterpret_cast<HashType&>(*working) == NetStreamHash);
			//buffer += sizeof(HashType);

			value.Deserialize(*this);
		}

		uint32 Read(char* value) {
			LengthType size;
			ReadBaseType(size);
			memcpy(value, buffer, size);
			buffer += size;
			return size;
		}
		uint32 Read(wchar_t* value) {
			LengthType size;
			ReadBaseType(size);
			memcpy(value, buffer, size_t(size) * size_t(sizeof(wchar_t)));
			buffer += size;
			return size;
		}
#pragma endregion
		void Read(std::string& value) { ReadString(value); }
		void Read(String& value) { ReadString(value); }
		void Read(std::vector<std::string>& value) {
			int vectorLenght = 0;
			Read(vectorLenght);
			value.resize(vectorLenght);
			std::string aux{};
			for (int i = 0; i < vectorLenght; i++) {
				ReadString(aux);
				value[i] = aux;
			}
		}
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
		uint16 GetNetStreamSize() {
			const LengthType* result = (const LengthType*)buffer;
			return *(result);
		}
	private:
		template<typename T>
		void ReadBaseType(T& value) {
			if (!CanRead(sizeof(T))) {
				throw new Exception("Cannot read Base Type past stream.");
			}
			memcpy(&value, buffer, sizeof(T));
			buffer += sizeof(T);
		}
		template<typename T>
		uint32 ReadString(T& value) {
			LengthType strSize;
			Read(strSize);
			if (!CanRead(sizeof(typename T::value_type) * strSize)) {
				throw new Exception("Cannot read string past stream.");
			}
			value = (typename T::value_type*)buffer;
			buffer += sizeof(typename T::value_type) * strSize;
			return strSize;
		}
		bool CanRead(uint16 size) {
			if (size + buffer > endOfBuffer) {
				ERROR_LOG("Cannot read past stream.", 0);
				return false;
			}
			return true;
		}
	protected:
		const uint8* buffer;
		const uint8* endOfBuffer;
	};
	class StreamWriter
	{
	public:
		StreamWriter(IBuffer& buffer) : buffer(buffer), internalBuffer(NULL, 0) {}
		StreamWriter(char* buffer, uint32 size)
			: buffer(internalBuffer), internalBuffer(buffer, size) {}
		void Write(Serializer::ISerializable& value) {
			buffer.Clear();
			if (buffer.GetMaxLength() < sizeof(LengthType)) {  // buffer overflow
				throw; // overflow
			}

			buffer.Append(sizeof(LengthType));  // reserve size
			buffer.Append(&NetStreamHash, sizeof(HashType)); /*adding E0 55 to it*/
			value.Serialize(*this);
			*(LengthType*)buffer.GetData() = (LengthType)buffer.GetLength();  // update size
		}
		void Write(const wchar_t* name, Serializer::ISerializable& value) {
			HashType hash = StringUtil::Hash(name);
			if (buffer.GetMaxLength() < sizeof(hash)) {  // buffer overflow
				throw;
			}
			buffer.Append(&hash, sizeof(HashType));
			value.Serialize(*this);
		}
		void Begin(uint16 protocolTypeHashed) {
			if (buffer.GetMaxLength() < sizeof(protocolTypeHashed))   // buffer overflow
				throw;
			buffer.Append(&protocolTypeHashed, sizeof(HashType));
		}
#pragma region Base Types
		void Write(uint8	value) { WriteBaseType(value); }
		void Write(uint16	value) { WriteBaseType(value); }
		void Write(uint32	value) { WriteBaseType(value); }
		void Write(uint64	value) { WriteBaseType(value); }
		void Write(int8		value) { WriteBaseType(value); }
		void Write(int16	value) { WriteBaseType(value); }
		void Write(int32	value) { WriteBaseType(value); }
		void Write(int64	value) { WriteBaseType(value); }
#pragma endregion
#pragma region String Related
		void Write(std::wstring& value) { WriteString(value); }
		void Write(std::string& value) { WriteString(value); }
		void Write(const char* value) { WriteString(std::string(value)); }
		void Write(const wchar_t* value) { WriteString(std::wstring(value)); }
#pragma endregion;
		char* GetData() { return buffer.GetData(); }
		uint32 GetLength() { return buffer.GetLength(); }
	private:
		template<typename T>
		void WriteString(const T& value) {
			LengthType size = (LengthType)value.size();
			size++; //null terminated.
			if ((unsigned int)buffer.GetMaxLength() < sizeof(LengthType) + size * sizeof(T::value_type))
				throw; //overflow
			buffer.Append(&size, sizeof(LengthType));
			buffer.Append(value.c_str(), sizeof(T::value_type) * size);
		}
		template <typename T>
		void WriteBaseType(T& value) { buffer.Append(&value, sizeof(T)); }
		IBuffer& buffer;
		Buffer internalBuffer;
	};
}

#ifndef NET_SERIALIZABLE
#define NET_SERIALIZABLE								    \
    static const String TypeName;							\
    static const uint16 TypeHash;							\
    void Serialize(Serializer::StreamWriter &out) override;	\
    void Deserialize(Serializer::StreamReader &in) override;\
    const String GetTypeName() { return TypeName; }
#endif

#endif // !Serializer_HEADER_GUARD
