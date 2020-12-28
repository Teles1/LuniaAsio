#include "Serializer.h"
#include <iostream>
namespace Lunia {
	namespace Serializer {

		void IStreamWriter::Write(const wchar_t* name, const std::string& value)
		{
			Write(name, value.c_str());
		}

		void IStreamWriter::Write(const wchar_t* name, const std::wstring& value)
		{
			Write(name, value.c_str());
		}

		void IStreamWriter::Write(const wchar_t* name, const float4x4& value)
		{
			Private::Matrix4x4Serialize<float4x4> helper(value);
			Write(name, static_cast<const ISerializable&>(helper));
		}

		void IStreamWriter::WriteRawData(const wchar_t*, const unsigned char*, unsigned int)
		{
			Logger::GetInstance().Exception("active serializer implementation doesn't support writing raw-data");
		};

		void IStreamReader::Begin(const wchar_t* type, Version& version)
		{
			int major = 0;
			int minor = 0;
			Begin(type, &major, &minor);
			version.Set(type, major, minor);
		}

		void IStreamReader::ReadRawData(const wchar_t*, unsigned char*, unsigned int)
		{
			Logger::GetInstance().Exception("active deserializer implementation doesn't support reading raw-data");
		}

		void IStreamReader::Read(const wchar_t* name, std::wstring& value)
		{
			wchar_t tmp[MaxString];
			int readed = Read(name, tmp, MaxString);
			assert(readed < MaxString);
			value = tmp;
			readed = 0;
		}

		void IStreamReader::Read(const wchar_t* name, std::string& value)
		{
			char tmp[MaxString];
			int readed = Read(name, tmp, MaxString);
			assert(readed < MaxString);
			value = tmp;
			readed = 0;
		}
		void IStreamReader::Read(const wchar_t* name, float4x4& value)
		{
			Private::Matrix4x4Deserialize<float4x4> helper(value);
			Read(name, static_cast<ISerializable&>(helper));
		}
	}
}
