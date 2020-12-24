#include "NetStream.h"
namespace Lunia {
	namespace Net {
		HashType Hash(const wchar_t* str) {
			unsigned int baseHash = StringUtil::Hash(str);
			return (HashType)baseHash;
		}
        /*
        NetStream structure

        bytes : contents
        00~01 : Size of NetStream
        02~03 : NetStreamId hash code
        04~05 : Type hash code
        06~   : Data
        */

        // StreamWriter
        // ///////////////////////////////////////////////////////////////////////////////
        StreamWriter::StreamWriter(void* buffer, int size)
            : buffer(internalBuffer), internalBuffer(buffer, size) {}

        StreamWriter::~StreamWriter() {}

        /*LengthType StreamWriter::EstimateSize(const Serializer::ISerializable& value) {
            SizeWriter s;
            s.Write(value);
            return s.GetSize();
        }*/

        void StreamWriter::Write(const Serializer::ISerializable& value) {
            buffer.Clear();
            if (buffer.GetMaxLength() < sizeof(LengthType)) {  // buffer overflow
                Logger::GetInstance().Exception("buffer overflow on reserving length");
            }

            buffer.Append(sizeof(LengthType));  // reserve size
            buffer.Append(&NetStreamHash, sizeof(NetStreamHash)); /*adding E0 55 to it*/
            value.Serialize(*this);
            *(LengthType*)buffer.GetData() = (LengthType)buffer.GetLength();  // update size
        }

        void StreamWriter::Append(
            const Serializer::ISerializable& value)  // 20051130 : ssanz
        {
            int appended = buffer.GetLength();
            buffer.Append(sizeof(LengthType));  // reserve size
            Write(NetStreamId, value);
            *(LengthType*)(buffer.GetData() + appended) =
                (LengthType)(buffer.GetLength() - appended);  // update size
        }

        Serializer::IStreamWriter::Type StreamWriter::GetType() const {
            return Serializer::IStreamWriter::Type::Flat;
        }

        void StreamWriter::Begin(const wchar_t* name, int /*majorVersion*/, int /*minorVersion*/) {
            /*Adding the HASH(name) to the buffer*/
            HashType type = Hash(name);
            if (name == L"AnswerSecurityCode")
                HashType type = 0xC52F;
            if (buffer.GetMaxLength() < sizeof(type)) {  // buffer overflow
                Logger::GetInstance().Exception("buffer overflow on hashing type");
            }
            buffer.Append(&type, sizeof(type));
        }

        void StreamWriter::Write(const wchar_t* name, const wchar_t* buffer) {
            WriteString(name, std::wstring(buffer));
        }

        void StreamWriter::Write(const wchar_t* name, const char* buffer) {
            WriteString(name, std::string(buffer));
        }

        void StreamWriter::Write(const wchar_t* name, const Locator& locator) {
            Write(name, locator.GetRelativePath());
        }

        void StreamWriter::Write(const wchar_t* name, bool value) {
            WriteBaseType(name, value);
        }

        void StreamWriter::Write(const wchar_t* name, char value) {
            WriteBaseType(name, value);
        }

        void StreamWriter::Write(const wchar_t* name, unsigned char value) {
            WriteBaseType(name, value);
        }

        void StreamWriter::Write(const wchar_t* name, signed short value) {
            WriteBaseType(name, value);
        }

        void StreamWriter::Write(const wchar_t* name, unsigned short value) {
            WriteBaseType(name, value);
        }

        void StreamWriter::Write(const wchar_t* name, int value) {
            WriteBaseType(name, value);
        }

        void StreamWriter::Write(const wchar_t* name, unsigned int value) {
            WriteBaseType(name, value);
        }

        void StreamWriter::Write(const wchar_t* name, __int64 value) {
            WriteBaseType(name, value);
        }

        void StreamWriter::Write(const wchar_t* name, unsigned __int64 value) {
            WriteBaseType(name, value);
        }

        void StreamWriter::Write(const wchar_t* name, double value) {
            WriteBaseType(name, value);
        }

        void StreamWriter::Write(const wchar_t* name, float value) {
            WriteBaseType(name, value);
        }

        void StreamWriter::Write(const wchar_t* name, const float2& value) {
            WriteBaseType(name, value);
        }

        void StreamWriter::Write(const wchar_t* name, const float3& value) {
            WriteBaseType(name, value);
        }

        void StreamWriter::Write(const wchar_t* name, const float4& value) {
            WriteBaseType(name, value);
        }

        void StreamWriter::Write(const wchar_t* name, const Serializer::ISerializable& value) {
            HashType hash = Hash(name);
            if (buffer.GetMaxLength() < sizeof(hash)) {  // buffer overflow
                Logger::GetInstance().Exception("buffer overflow on hashing value");
            }

            buffer.Append(&hash, sizeof(hash));
            value.Serialize(*this);
        }
        ///////////////////////////////////////////////////////////////////////////////
        ///StreamWriter //

        // StreamReader
        // ///////////////////////////////////////////////////////////////////////////////
        StreamReader::StreamReader(const void* buffer) : 
            buffer(buffer)
            , endOfBuffer(static_cast<const char*>(buffer) + GetNetStreamSize()) 
        {
            size = GetNetStreamSize();
        }

        StreamReader::~StreamReader() {}

        int StreamReader::GetNetStreamHeaderSize() {
            /* NetStream header consists of "NetStreamId hash" + "Size of struct which
             * is serialized" */
            return sizeof(HashType) + sizeof(LengthType);
        }

        int StreamReader::GetNetStreamSize() {
            const LengthType* unit = (const LengthType*)buffer;
            return *(unit);
        }

        bool StreamReader::IsNetStream() {
            char* check = (char*)buffer + sizeof(LengthType);
            return *(HashType*)check == NetStreamHash ? true : false;
        }

        HashType StreamReader::GetSerializedTypeHash() {
            HashType* temp = (HashType*)((const char*)buffer + GetNetStreamHeaderSize());

            return *temp;
        }

        Serializer::IStreamReader::Type StreamReader::GetType() const {
            return IStreamReader::Type::Flat;
        }
        size_t StreamReader::GetCount(const wchar_t*) const {
            Logger::GetInstance().Exception("unsurppoted method in flat stream");
            return 0;
        }

        void StreamReader::Read(Serializer::ISerializable& value) {
            working = (char*)buffer;  // buffer size should be checked before using this method
            working += sizeof(LengthType);  // skip length information of NetStream

            // check hash
            assert(*(HashType*)working == NetStreamHash);
            working += sizeof(HashType);

            value.Deserialize(*this);
        }

        void StreamReader::Begin(const wchar_t* /*type*/, int* /*majorVersion*/, int* /*minorVersion*/) {
            working += sizeof(HashType);
        }

        bool StreamReader::Exists(const wchar_t* /*name*/) const {
            Logger::GetInstance().Exception("unsurppoted method in flat stream");
            return false;
        }

        void StreamReader::Read(const wchar_t* name, Locator& value) {
            std::wstring path;
            ReadString(name, path);
            value = Locator(path.c_str());
        }

        void StreamReader::Read(const wchar_t* name, bool& value) {
            ReadBaseType(name, value);
        }

        void StreamReader::Read(const wchar_t* name, char& value) {
            ReadBaseType(name, value);
        }

        void StreamReader::Read(const wchar_t* name, unsigned char& value) {
            ReadBaseType(name, value);
        }

        void StreamReader::Read(const wchar_t* name, signed short& value) {
            ReadBaseType(name, value);
        }

        void StreamReader::Read(const wchar_t* name, unsigned short& value) {
            ReadBaseType(name, value);
        }

        void StreamReader::Read(const wchar_t* name, int& value) {
            ReadBaseType(name, value);
        }

        void StreamReader::Read(const wchar_t* name, unsigned int& value) {
            ReadBaseType(name, value);
        }

        void StreamReader::Read(const wchar_t* name, __int64& value) {
            ReadBaseType(name, value);
        }

        void StreamReader::Read(const wchar_t* name, unsigned __int64& value) {
            ReadBaseType(name, value);
        }

        void StreamReader::Read(const wchar_t* name, double& value) {
            ReadBaseType(name, value);
        }

        void StreamReader::Read(const wchar_t* name, float& value) {
            ReadBaseType(name, value);
        }

        void StreamReader::Read(const wchar_t* name, float2& value) {
            ReadBaseType(name, value);
        }

        void StreamReader::Read(const wchar_t* name, float3& value) {
            ReadBaseType(name, value);
        }

        void StreamReader::Read(const wchar_t* name, float4& value) {
            ReadBaseType(name, value);
        }
        // Client crap
        void StreamReader::Read(const wchar_t* name, Serializer::ISerializable& value) {
            HashType hash = Hash(name);
            Logger::GetInstance().Info("name[{0}] hash[{1}]", StringUtil::ToASCII(name), hash);
            HashType check = *(HashType*)working;
            if (hash != check) {
                Logger::GetInstance().Exception("NetStream value mismatched name:{0}(hash {1}) is "
                    "not{2} [stream size:{3}, currentPosition:{4}]",  StringUtil::ToASCII(name), hash, check, GetNetStreamSize(),  (int)((char*)working - (char*)buffer));
            }
            working += sizeof(hash);

            try {
                value.Deserialize(*this);
            }
            catch (...) {
                Logger::GetInstance().Exception("buffer overflow detected on deserializing {0}({1})", StringUtil::ToASCII(name), hash);
            }
        }

        unsigned int StreamReader::Read(const wchar_t* name, char* buffer, unsigned int size) {
            std::string str;
            ReadString(name, str);
            strncpy_s(buffer, size, str.c_str(), size - 1);
            buffer[size - 1] = 0;

            return (unsigned int)str.size();
        }

        unsigned int StreamReader::Read(const wchar_t* name, wchar_t* buffer, unsigned int size) {
            std::wstring str;
            ReadString(name, str);
            wcsncpy_s(buffer, size, str.c_str(), size - 1);
            buffer[size - 1] = 0;

            return (unsigned int)str.size();
        }
        ///////////////////////////////////////////////////////////////////////////////
        ///StreamReader //
	}
}