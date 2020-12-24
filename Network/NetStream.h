#pragma once
#include "Core/Serializer/Serializer.h"
#include "Core/Utils/Buffer.h"

namespace Lunia {
	namespace Net {
        HashType Hash(const wchar_t* str);
        static const wchar_t* NetStreamId = L"NetStream";
        static const HashType NetStreamHash = Hash(NetStreamId);
        class StreamWriter : public Serializer::IStreamWriter {
        public:
            StreamWriter(void* buffer, int size);
            StreamWriter(IBuffer& buffer) : buffer(buffer), internalBuffer(NULL, 0) {}
            virtual ~StreamWriter();

            virtual void Write(const Serializer::ISerializable& value);
            virtual void Append(const Serializer::ISerializable& value);
            virtual int GetLength() const { return buffer.GetLength(); }
            //static LengthType EstimateSize(const Serializer::ISerializable& value);

        protected:  // Serializer::IStreamWriter implementation
            virtual Serializer::IStreamWriter::Type GetType() const;
            virtual void Begin(const wchar_t* name, int majorVersion, int minorVersion);
            virtual void Write(const wchar_t* name, const wchar_t* buffer);
            virtual void Write(const wchar_t* name, const char* buffer);
            virtual void Write(const wchar_t* name, const Locator& locator);
            virtual void Write(const wchar_t* name, bool value);
            virtual void Write(const wchar_t* name, char value);
            virtual void Write(const wchar_t* name, unsigned char value);
            virtual void Write(const wchar_t* name, signed short value);
            virtual void Write(const wchar_t* name, unsigned short value);
            virtual void Write(const wchar_t* name, int value);
            virtual void Write(const wchar_t* name, unsigned int value);
            virtual void Write(const wchar_t* name, __int64 value);
            virtual void Write(const wchar_t* name, unsigned __int64 value);
            virtual void Write(const wchar_t* name, double value);
            virtual void Write(const wchar_t* name, float value);
            virtual void Write(const wchar_t* name, const float2& value);
            virtual void Write(const wchar_t* name, const float3& value);
            virtual void Write(const wchar_t* name, const float4& value);
            virtual void Write(const wchar_t* name, const Serializer::ISerializable& value);

        private:
            StreamWriter& operator=(const StreamWriter& /*sender*/) {
                return *this;
            }  // unable to copy

            template <typename T>
            void WriteBaseType(const wchar_t* /*name*/, T& value) {
                buffer.Append(&value, sizeof(value));
            }

            template <typename T>
            void WriteString(const wchar_t* /*name*/, const T& value) {
                LengthType size = (LengthType)value.size();
                ++size;  // end of string

                if ((unsigned int)buffer.GetMaxLength() <
                    sizeof(size) + size * sizeof(T::value_type)) {
                    Logger::GetInstance().Exception("buffer overflow at writing string type");
                }

                buffer.Append(&size, sizeof(size));
                buffer.Append(value.c_str(), size * sizeof(T::value_type));
            }

            Buffer internalBuffer;
            IBuffer& buffer;
        };

        class StreamReader : public Serializer::IStreamReader {
        public:
            StreamReader(const void* buffer);
            virtual ~StreamReader();

            static int GetNetStreamHeaderSize();

            // virtual static bool IsNetStream(const void* buffer);
            // virtual static int GetNetStreamSize(const void* buffer);
            virtual HashType GetSerializedTypeHash();
            virtual bool IsNetStream();
            virtual int GetNetStreamSize();
            virtual void Read(Serializer::ISerializable& value);

        protected:  // Serializer::IStreamWriter implementation
            virtual Serializer::IStreamReader::Type GetType() const;
            virtual size_t GetCount(const wchar_t* type) const;
            virtual void Begin(const wchar_t* type, int* majorVersionOut, int* minorVersionOut);
            virtual bool Exists(const wchar_t* name) const;
            virtual void Read(const wchar_t* name, Locator& value);
            virtual void Read(const wchar_t* name, bool& value);
            virtual void Read(const wchar_t* name, char& value);
            virtual void Read(const wchar_t* name, unsigned char& value);
            virtual void Read(const wchar_t* name, signed short& value);
            virtual void Read(const wchar_t* name, unsigned short& value);
            virtual void Read(const wchar_t* name, int& value);
            virtual void Read(const wchar_t* name, unsigned int& value);
            virtual void Read(const wchar_t* name, __int64& value);
            virtual void Read(const wchar_t* name, unsigned __int64& value);
            virtual void Read(const wchar_t* name, double& value);
            virtual void Read(const wchar_t* name, float& value);
            virtual void Read(const wchar_t* name, float2& value);
            virtual void Read(const wchar_t* name, float3& value);
            virtual void Read(const wchar_t* name, float4& value);
            virtual void Read(const wchar_t* name, Serializer::ISerializable& value);
            virtual unsigned int Read(const wchar_t* name, char* buffer,  unsigned int size);
            virtual unsigned int Read(const wchar_t* name, wchar_t* buffer, unsigned int size);

        private:
            template <typename T>
            void ReadBaseType(const wchar_t* /*name*/, T& value) {
                if (working + sizeof(value) > endOfBuffer)
                    Logger::GetInstance().Exception("buffer overflow at reading basic type");
                memcpy(&value, working, sizeof(value));
                working += sizeof(value);
            }

            template <typename T>
            void ReadString(const wchar_t* /*name*/, T& value) {
                if (working + sizeof(LengthType) > endOfBuffer)
                    Logger::GetInstance().Exception("buffer overflow at reading string size");
                LengthType size = *(LengthType*)working;
                working += sizeof(size);
                if (working + size * sizeof(typename T::value_type) > endOfBuffer)
                    Logger::GetInstance().Exception("buffer overflow at reading string");
                value = (typename T::value_type*)working;
                working += (size * sizeof(typename T::value_type));
            }

            const void* buffer;
            char* working;
            const void* endOfBuffer;
        public:
            //need size to avoid the strcpy warning so we can safely check for overflows.
            uint32 size;
            void PrintOutData() {
                for (uint32 i = 0; i < size; i++)
                    printf("%.2X", ((unsigned char*)buffer)[i]);
                printf("\n");
            }
        };

	}
}

