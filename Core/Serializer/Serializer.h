#pragma once
#include <string>
#include <map>
#include <list>
#include <vector>
#include <set>
#include <deque>

#include "IReferenceCounted.h"
#include <Core/Utils/Math/Math.h>
#include <Core/Utils/StringUtil.h>
#include <Core/Utils/Locator.h>

#include <Core/Utils/Version.h>
#include <Core/com_ptr.h>
#include <Core/IStream.h>

namespace Lunia {
	namespace Serializer {
		class IStreamWriter;
		class IStreamReader;

		class ISerializable
		{

		public:
			virtual void Serialize(IStreamWriter& out) const = 0;
			virtual void Deserialize(IStreamReader& in) = 0;

			virtual ~ISerializable() {}
		};

		#include <Core/Serializer/PrivateFlat.h>
		#include <Core/Serializer/Private.h>
		
		class IStreamWriter
		{

			template <typename T>
			void writeContainer(const wchar_t* name, const T& container)
			{
				if (GetType() == Type::Flat)
				{
					PrivateFlat::ContainerSerialize<T> helper(container);
					Write(name, helper);
				}
				else
				{
					Private::ContainerSerialize<T> helper(container);
					Write(name, helper);
				}
			}

			template <typename T>
			void writeKeyedContainer(const wchar_t* name, const T& container)
			{
				if (GetType() == Type::Flat)
				{
					PrivateFlat::KeyedContainerSerialize<T> helper(container);
					Write(name, helper);
				}
				else
				{
					Private::KeyedContainerSerialize<T> helper(container);
					Write(name, helper);
				}
			}

		public:
			virtual ~IStreamWriter() {}

			enum Type
			{
				Structured, //Xml, Structured Binary
				Flat,		//Binary
			};

			/*enum Flags {
					RandomAccess=1
					};*/
			virtual Type GetType() const = 0;
			/**
					must be called before any calls to write-methods,if the method is not called, the class type is unknown

					@param name typename of the class to be serialized
					*/
			virtual void Begin(const wchar_t* name, int majorVersion = 0, int minorVersion = 0) = 0;
			/**
					write-methods for the primitive data types supported
					@param name name of the parameter
					@value value of the parameter
					*/
			//virtual void Write(const wchar_t* name, const Locator& locator) = 0;
			virtual void Write(const wchar_t* name, bool value) = 0;
			virtual void Write(const wchar_t* name, char value) = 0;
			virtual void Write(const wchar_t* name, unsigned char value) = 0;
			virtual void Write(const wchar_t* name, signed short value) = 0;
			virtual void Write(const wchar_t* name, unsigned short value) = 0;
			virtual void Write(const wchar_t* name, int value) = 0;
			virtual void Write(const wchar_t* name, unsigned int value) = 0;
			virtual void Write(const wchar_t* name, __int64 value) = 0;
			virtual void Write(const wchar_t* name, unsigned __int64 value) = 0;
			virtual void Write(const wchar_t* name, double value) = 0;
			virtual void Write(const wchar_t* name, float value) = 0;
			virtual void Write(const wchar_t* name, const float2& value) = 0;
			virtual void Write(const wchar_t* name, const float3& value) = 0;
			virtual void Write(const wchar_t* name, const float4& value) = 0;
			virtual void Write(const wchar_t* name, const wchar_t* buffer) = 0;
			virtual void Write(const wchar_t* name, const char* buffer) = 0;
			virtual void Write(const wchar_t* name, const ISerializable& value) = 0;

			virtual void Write(const wchar_t* name, const std::string& value) { Write(name, value.c_str()); }
			virtual void Write(const wchar_t* name, const std::wstring& value) { Write(name, value.c_str()); }
			template <int Size>
			void Write(const wchar_t* name, const static_string<Size>& value)
			{
				Write(name, value.c_str());
			}
			template <int Size>
			void Write(const wchar_t* name, const static_wstring<Size>& value)
			{
				Write(name, value.c_str());
			}

			void Write(const wchar_t* name, const float4x4& value)
			{
				Lunia::Serializer::Private::Matrix4x4Serialize<float4x4> helper(value);
				Write(name, static_cast<const ISerializable&>(helper));
			}

			template <typename T>
			void Write(const wchar_t* name, const T& value)
			{
				Private::WriterWrapper<const T> helper(value);
				Write(name, static_cast<const ISerializable&>(helper));
			}

			template <typename T>
			void WriteArray(const wchar_t* name, const T* array, unsigned int size)
			{
				if (GetType() == Type::Flat)
				{
					PrivateFlat::ArraySerialize<T> helper(array, size);
					Write(name, helper);
				}
				else
				{
					Private::ArraySerialize<T> helper(array, size);
					Write(name, helper);
				}
			}

			virtual void WriteRawData(const wchar_t* /*name*/, const unsigned char* /*buffer*/, unsigned int /*size*/) { 
				Logger::GetInstance()->Exception("active serializer implementation doesn't support writing raw-data"); 
			};

			/**
					write-methods for the std containers
					*/
			template <typename T>
			void Write(const wchar_t* name, const std::vector<T>& value) { writeContainer(name, value); }
			template <typename T>
			void Write(const wchar_t* name, const std::list<T>& value) { writeContainer(name, value); }
			template <typename T>
			void Write(const wchar_t* name, const std::deque<T>& value) { writeContainer(name, value); }
			template <typename T>
			void Write(const wchar_t* name, const std::set<T>& value) { writeContainer(name, value); }
			template <typename T1, typename T2>
			void Write(const wchar_t* name, const std::set<T1, T2>& value) { writeContainer(name, value); }
			template <typename T1, typename T2>
			void Write(const wchar_t* name, const std::map<T1, T2>& value) { writeKeyedContainer(name, value); }
			template <typename T1, typename T2, typename T3>
			void Write(const wchar_t* name, const std::map<T1, T2, T3>& value) { writeKeyedContainer(name, value); }
			template <typename T1, typename T2>
			void Write(const wchar_t* name, const std::pair<T1, T2>& value)
			{
				Private::Pair<T1, T2> helper(value.first, value.second);
				Write(name, /*static_cast<const ISerializable&>*/ (helper));
			}

			template <typename T>
			void WriteEnum(const wchar_t* name, const T& value)
			{
				uint32 tmp = static_cast<uint32>(value);
				Write(name, tmp);
			}
		};

		class IStreamReader
		{

			enum
			{
				MaxString = 1024
			};

			template <typename T>
			void readContainer(const wchar_t* name, T& container)
			{
				if (GetType() == Type::Flat)
				{
					PrivateFlat::ContainerDeserialize<T> helper(container);
					Read(name, helper);
				}
				else
				{
					Private::ContainerDeserialize<T> helper(container);
					Read(name, helper);
				}
			}

			template <typename T>
			void readSetContainer(const wchar_t* name, T& container)
			{
				if (GetType() == Type::Flat)
				{
					PrivateFlat::SetContainerDeserialize<T> helper(container);
					Read(name, helper);
				}
				else
				{
					Private::SetContainerDeserialize<T> helper(container);
					Read(name, helper);
				}
			}

			template <typename T>
			void readKeyedContainer(const wchar_t* name, T& container)
			{
				if (GetType() == Type::Flat)
				{
					PrivateFlat::KeyedContainerDeserialize<T> helper(container);
					Read(name, helper);
				}
				else
				{
					Private::KeyedContainerDeserialize<T> helper(container);
					Read(name, helper);
				}
			}

		public:
			virtual ~IStreamReader() {}

			enum Type
			{
				Structured, //xml
				Flat,		//binary
			};

			virtual Type GetType() const = 0;

			virtual size_t GetCount(const wchar_t* type) const = 0;

			virtual void Begin(const wchar_t* type, int* majorVersionOut = 0, int* minorVersionOut = 0) = 0;

			void Begin(const wchar_t* type, Version& version)
			{
				int major = 0;
				int minor = 0;
				Begin(type, &major, &minor);
				version.Set(type, major, minor);
			}
			virtual bool Exists(const wchar_t* name) const = 0;
			virtual void Read(const wchar_t* name, Locator& value) = 0;
			virtual void Read(const wchar_t* name, bool& value) = 0;
			virtual void Read(const wchar_t* name, char& value) = 0;
			virtual void Read(const wchar_t* name, unsigned char& value) = 0;
			virtual void Read(const wchar_t* name, signed short& value) = 0;
			virtual void Read(const wchar_t* name, unsigned short& value) = 0;
			virtual void Read(const wchar_t* name, int& value) = 0;
			virtual void Read(const wchar_t* name, unsigned int& value) = 0;
			virtual void Read(const wchar_t* name, __int64& value) = 0;
			virtual void Read(const wchar_t* name, unsigned __int64& value) = 0;
			virtual void Read(const wchar_t* name, double& value) = 0;
			virtual void Read(const wchar_t* name, float& value) = 0;
			virtual void Read(const wchar_t* name, float2& value) = 0;
			virtual void Read(const wchar_t* name, float3& value) = 0;
			virtual void Read(const wchar_t* name, float4& value) = 0;
			virtual void Read(const wchar_t* name, ISerializable& value) = 0;
			virtual unsigned int Read(const wchar_t* name, char* buffer, unsigned int size) = 0;
			virtual unsigned int Read(const wchar_t* name, wchar_t* buffer, unsigned int size) = 0;

			virtual void ReadRawData(const wchar_t* /*name*/, unsigned char* /*buffer*/, unsigned int /*size*/) { 
				Logger::GetInstance()->Exception("active deserializer implementation doesn't support reading raw-data"); 
			};

			template <int Size>
			void Read(const wchar_t* name, static_wstring<Size>& value)
			{
				std::vector<wchar_t> buffer(value.max_size());
				Read(name, &buffer[0], uint32(value.max_size()));
				value = &buffer[0];
				//Read(name,value.c_str(),unsigned int(value.max_size()));
			}
			template <int Size>
			void Read(const wchar_t* name, static_string<Size>& value)
			{
				std::vector<char> buffer(value.max_size());
				Read(name, &buffer[0], uint32(value.max_size()));
				value = &buffer[0];
				//Read(name,value.c_str(),unsigned int(value.max_size()));
			}
			void Read(const wchar_t* name, std::wstring& value)
			{
				wchar_t tmp[MaxString];
				int readed = Read(name, tmp, MaxString);
				assert(readed < MaxString);
				value = tmp;
				readed = 0;
			}

			void Read(const wchar_t* name, std::string& value)
			{
				char tmp[MaxString];
				int readed = Read(name, tmp, MaxString);
				assert(readed < MaxString);
				value = tmp;
				readed = 0;
			}

			void Read(const wchar_t* name, float4x4& value)
			{
				Private::Matrix4x4Deserialize<float4x4> helper(value);
				Read(name, static_cast<ISerializable&>(helper));
			}

			template <typename T>
			void Read(const wchar_t* name, T& value)
			{
				Private::ReaderWrapper<T> helper(value);
				Read(name, static_cast<ISerializable&>(helper));
			}

			template <typename T>
			void ReadArray(const wchar_t* name, T* array, unsigned int size)
			{
				if (GetType() == Type::Flat)
				{
					PrivateFlat::ArrayDeserialize<T> helper(array, size);
					Read(name, helper);
				}
				else
				{
					Private::ArrayDeserialize<T> helper(array, size);
					Read(name, helper);
				}
			}

			template <typename T>
			void Read(const wchar_t* name, std::vector<T>& value)
			{
				value.clear();
				readContainer(name, value);
			}
			template <typename T>
			void Read(const wchar_t* name, std::list<T>& value)
			{
				value.clear();
				readContainer(name, value);
			}
			template <typename T>
			void Read(const wchar_t* name, std::deque<T>& value)
			{
				value.clear();
				readContainer(name, value);
			}
			template <typename T>
			void Read(const wchar_t* name, std::set<T>& value)
			{
				value.clear();
				readSetContainer(name, value);
			}
			template <typename T1, typename T2>
			void Read(const wchar_t* name, std::set<T1, T2>& value)
			{
				value.clear();
				readSetContainer(name, value);
			}

			template <typename T1, typename T2>
			void Read(const wchar_t* name, std::map<T1, T2>& value)
			{
				value.clear();
				readKeyedContainer(name, value);
			}
			template <typename T1, typename T2, typename T3>
			void Read(const wchar_t* name, std::map<T1, T2, T3>& value)
			{
				value.clear();
				readKeyedContainer(name, value);
			}
			template <typename T1, typename T2, typename T3>
			void Read(const wchar_t* name, std::map<T1, T2, T3>& value, bool clear)
			{
				if (clear) {
					value.clear();
				}
				readKeyedContainer(name, value);
			}
			template <typename T1, typename T2>
			void Read(const wchar_t* name, std::pair<T1, T2>& value)
			{
				//value.clear();
				Private::Pair<T1, T2> helper;
				Read(name, helper);

				value = helper.Value;
			}

			template <typename T>
			void Read(const wchar_t* name, T& value, const T& defaultValue)
			{
				if (GetType() == Type::Flat)
				{
					Read(name, value);
				}
				else
				{
					if (Exists(name))
					{
						Read(name, value);
					}
					else
					{
						value = defaultValue;
					}
				}
			}

			template <typename T>
			void ReadEnum(const wchar_t* name, T& value, const T& defaultValue)
			{
				uint32 tmp;
				Read(name, tmp, uint32(defaultValue));
				value = static_cast<T>(tmp);
			}

			template <typename T>
			void ReadEnum(const wchar_t* name, T& value)
			{
				uint32 tmp;
				Read(name, tmp);
				value = static_cast<T>(tmp);
			}
		};

		class IRefCountedStreamWriter : public IStreamWriter, public IReferenceCounted
		{
		};
		class IRefCountedStreamReader : public IStreamReader, public IReferenceCounted
		{
		};

	}
}
#ifndef NET_SERIALIZABLE
#define NET_SERIALIZABLE                                          \
    static const wchar_t *TypeName;                               \
    static const Lunia::HashType TypeHash;						  \
    virtual void Serialize(Serializer::IStreamWriter &out) const; \
    virtual void Deserialize(Serializer::IStreamReader &in);      \
    const wchar_t *GetTypeName() { return TypeName; }
#endif