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
		
		class IStreamWriter
		{
			template <typename T>
			void writeContainer(const wchar_t* name, const T& container);

			template <typename T>
			void writeKeyedContainer(const wchar_t* name, const T& container);
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
			virtual void Write(const wchar_t* name, const Locator& locator) = 0;
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

			virtual void Write(const wchar_t* name, const std::string& value);
			virtual void Write(const wchar_t* name, const std::wstring& value);

			template <int Size>
			void Write(const wchar_t* name, const static_string<Size>& value);

			template <int Size>
			void Write(const wchar_t* name, const static_wstring<Size>& value);
			void Write(const wchar_t* name, const float4x4& value);

			template <typename T>
			void Write(const wchar_t* name, const T& value);

			template <typename T>
			void WriteArray(const wchar_t* name, const T* array, unsigned int size);

			virtual void WriteRawData(const wchar_t* /*name*/, const unsigned char* /*buffer*/, unsigned int /*size*/);

			template <typename T>
			void Write(const wchar_t* name, const std::vector<T>& value);
			template <typename T>
			void Write(const wchar_t* name, const std::list<T>& value);
			template <typename T>
			void Write(const wchar_t* name, const std::deque<T>& value);
			template <typename T>
			void Write(const wchar_t* name, const std::set<T>& value);
			template <typename T1, typename T2>
			void Write(const wchar_t* name, const std::set<T1, T2>& value);
			template <typename T1, typename T2>
			void Write(const wchar_t* name, const std::map<T1, T2>& value);
			template <typename T1, typename T2, typename T3>
			void Write(const wchar_t* name, const std::map<T1, T2, T3>& value);
			template <typename T1, typename T2>
			void Write(const wchar_t* name, const std::pair<T1, T2>& value);

			template <typename T>
			void WriteEnum(const wchar_t* name, const T& value);
		};
		class IStreamReader
		{

			enum
			{
				MaxString = 2 << 10
			};

			template <typename T>
			void readContainer(const wchar_t* name, T& container);

			template <typename T>
			void readSetContainer(const wchar_t* name, T& container);

			template <typename T>
			void readKeyedContainer(const wchar_t* name, T& container);

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

			void Begin(const wchar_t* type, Version& version);
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

			virtual void ReadRawData(const wchar_t* /*name*/, unsigned char* /*buffer*/, unsigned int /*size*/);

			template <int Size>
			void Read(const wchar_t* name, static_wstring<Size>& value);
			template <int Size>
			void Read(const wchar_t* name, static_string<Size>& value);

			void Read(const wchar_t* name, std::wstring& value);

			void Read(const wchar_t* name, std::string& value);

			void Read(const wchar_t* name, float4x4& value);

			template <typename T>
			void Read(const wchar_t* name, T& value);

			template <typename T>
			void ReadArray(const wchar_t* name, T* array, unsigned int size);

			template <typename T>
			void Read(const wchar_t* name, std::vector<T>& value);
			template <typename T>
			void Read(const wchar_t* name, std::list<T>& value);
			template <typename T>
			void Read(const wchar_t* name, std::deque<T>& value);
			template <typename T>
			void Read(const wchar_t* name, std::set<T>& value);
			template <typename T1, typename T2>
			void Read(const wchar_t* name, std::set<T1, T2>& value);

			template <typename T1, typename T2>
			void Read(const wchar_t* name, std::map<T1, T2>& value);
			template <typename T1, typename T2, typename T3>
			void Read(const wchar_t* name, std::map<T1, T2, T3>& value);
			template <typename T1, typename T2, typename T3>
			void Read(const wchar_t* name, std::map<T1, T2, T3>& value, bool clear);
			template <typename T1, typename T2>
			void Read(const wchar_t* name, std::pair<T1, T2>& value);

			template <typename T>
			void Read(const wchar_t* name, T& value, const T& defaultValue);

			template <typename T>
			void ReadEnum(const wchar_t* name, T& value, const T& defaultValue);

			template <typename T>
			void ReadEnum(const wchar_t* name, T& value);
		};

		
		namespace PrivateFlat {

			template <typename T>
			class ArraySerialize : public ISerializable {
				const T* buffer;
				unsigned int size;
			public:
				ArraySerialize(const T* pBuffer, unsigned int pSize) :buffer(pBuffer), size(pSize) {}

				virtual void Serialize(IStreamWriter& out) const {
					out.Begin(L"array");
					out.Write(L"size", size);
					for (unsigned int i = 0; i < size; ++i) {
						out.Write((L"item_" + StringUtil::To<std::wstring>(i)).c_str(), buffer[i]);
					}
				}
				virtual void Deserialize(IStreamReader&) { assert(false); }
			};

			template <typename T>
			class ArrayDeserialize : public ISerializable {
				T* buffer;
				unsigned int size;
			public:
				ArrayDeserialize(T* pBuffer, unsigned int pSize) :buffer(pBuffer), size(pSize) {}

				virtual void Serialize(IStreamWriter&) const {
					assert(false);
				}
				virtual void Deserialize(IStreamReader& in) {
					unsigned int fsize;
					in.Begin(L"array");
					in.Read(L"size", fsize);
					if (fsize >= size) {
						if (fsize != size)
							Logger::GetInstance().Warn("static array being deserialized has bigger size on file");
					}
					else {
						Logger::GetInstance().Exception(L"static array being deserialized has smaller size on file");
					}

					for (unsigned int i = 0; i < size; ++i) {
						T value;
						in.Read((L"item_" + StringUtil::To<std::wstring>(i)).c_str(), value);
						buffer[i] = value;
					}
				}

			};

			template <typename T>
			class ContainerSerialize : public ISerializable {
				const T& container;
			public:
				ContainerSerialize<T>(const T& pContainer) : container(pContainer) {}

				virtual void Serialize(IStreamWriter& out) const {
					out.Begin(L"array");
					out.Write(L"size", uint32(container.size()));
					int counter = 0;
					for (auto it = container.begin(); it != container.end(); ++it, ++counter) {
						out.Write((L"item_" + StringUtil::To<std::wstring>(counter)).c_str(), *it);
					}
				}
				virtual void Deserialize(IStreamReader&) { assert(false); }
			};

			template <typename T>
			class ContainerDeserialize : public ISerializable {
				T& container;
			public:
				ContainerDeserialize<T>(T& pContainer) : container(pContainer) {}

				virtual void Serialize(IStreamWriter&) const { assert(false); }
				virtual void Deserialize(IStreamReader& in) {
					in.Begin(L"array");
					unsigned int size;
					in.Read(L"size", size);

					for (unsigned int i = 0; i < size; ++i) {
						typename T::value_type value;
						in.Read((L"item_" + StringUtil::To<std::wstring>(i)).c_str(), value);
						container.push_back(value);
					}
				}
			};

			template <typename T>
			class SetContainerDeserialize : public ISerializable {
				T& container;
			public:
				SetContainerDeserialize<T>(T& pContainer) : container(pContainer) {}

				virtual void Serialize(IStreamWriter&) const { assert(false); }
				virtual void Deserialize(IStreamReader& in) {
					in.Begin(L"array");
					unsigned int size;
					in.Read(L"size", size);

					for (unsigned int i = 0; i < size; ++i) {
						typename T::value_type value;
						in.Read((L"item_" + StringUtil::To<std::wstring>(i)).c_str(), value);
						container.insert(value);
					}
				}
			};

			template <typename T>
			class KeyedContainerSerialize : public ISerializable {
				const T& container;
			public:
				KeyedContainerSerialize<T>(const T& pContainer) : container(pContainer) {}

				virtual void Serialize(IStreamWriter& out) const {
					out.Begin(L"associative_array");
					out.Write(L"size", uint32(container.size()));
					int counter = 0;
					for (auto it = container.begin(); it != container.end(); ++it, ++counter) {
						out.Write((L"item_first_" + StringUtil::To<std::wstring>(counter)).c_str(), it->first);
						out.Write((L"item_second_" + StringUtil::To<std::wstring>(counter)).c_str(), it->second);
					}
				}
				virtual void Deserialize(IStreamReader&) { assert(false); }
			};

			template <typename T>
			class KeyedContainerDeserialize : public ISerializable {
				T& container;
			public:
				KeyedContainerDeserialize<T>(T& pContainer) : container(pContainer) {}

				virtual void Serialize(IStreamWriter&) const { assert(false); }
				virtual void Deserialize(IStreamReader& in) {
					in.Begin(L"associative_array");
					unsigned int size;
					in.Read(L"size", size);

					for (unsigned int i = 0; i < size; ++i) {

						std::pair<T::key_type, T::mapped_type> value;
						in.Read((L"item_first_" + StringUtil::To<std::wstring>(i)).c_str(), value.first);
						in.Read((L"item_second_" + StringUtil::To<std::wstring>(i)).c_str(), value.second);
						container.insert(value);
					}

				}
			};


		} // namespace PrivateFlat

		namespace Private {
			template <typename T>
			class ArraySerialize : public ISerializable {
				const T* buffer;
				unsigned int size;
			public:
				ArraySerialize(const T* pBuffer, unsigned int pSize) :buffer(pBuffer), size(pSize) {}

				virtual void Serialize(IStreamWriter& out) const {
					out.Begin(L"array");
					for (unsigned int i = 0; i < size; ++i) {
						out.Write(L"item", buffer[i]);
					}
				}
				virtual void Deserialize(IStreamReader&) { assert(false); }
			};

			template <typename T>
			class ArrayDeserialize : public ISerializable {
				T* buffer;
				unsigned int size;
			public:
				ArrayDeserialize(T* pBuffer, unsigned int pSize) :buffer(pBuffer), size(pSize) {}

				virtual void Serialize(IStreamWriter&) const {
					assert(false);
				}
				virtual void Deserialize(IStreamReader& in) {
					in.Begin(L"array");
					size_t fsize = in.GetCount(L"item");
					if (fsize >= size) {
						if (fsize != size)
							Logger::GetInstance().Warn("static array being deserialized has bigger size on file");
					}
					else {
						Logger::GetInstance().Exception("static array being deserialized has smaller size on file");
					}

					for (unsigned int i = 0; i < size; ++i) {
						T value;
						in.Read(L"item", value);
						buffer[i] = value;
					}
				}

			};

			template <typename T>
			class ContainerSerialize : public ISerializable {
				const T& container;
			public:
				ContainerSerialize<T>(const T& pContainer) : container(pContainer) {}

				virtual void Serialize(IStreamWriter& out) const {
					out.Begin(L"array");
					int counter = 0;
					for (auto it = container.begin(); it != container.end(); ++it, ++counter) {
						out.Write(L"item", *it);
					}
				}
				virtual void Deserialize(IStreamReader&) { assert(false); }
			};

			template <typename T1, typename T2>
			class Pair : public ISerializable {
			public:

				std::pair<T1, T2> Value;


				Pair<T1, T2>(const T1& key, const T2& value) : Value(key, value) {}
				Pair<T1, T2>() {}

				virtual void Serialize(IStreamWriter& out) const {
					out.Begin(L"pair", 0, 0);
					out.Write(L"key", Value.first);
					out.Write(L"value", Value.second);

				}
				virtual void Deserialize(IStreamReader& in) {
					in.Begin(L"pair", 0, 0);
					in.Read(L"key", Value.first);
					in.Read(L"value", Value.second);
				}

			};

			template <typename T>
			class KeyedContainerSerialize : public ISerializable {
				const T& container;
			public:
				KeyedContainerSerialize<T>(const T& pContainer) : container(pContainer) {}

				virtual void Serialize(IStreamWriter& out) const {
					out.Begin(L"associative_array");
					//out.Write(L"size", unsigned int(container.size()));
					int counter = 0;
					for (auto it = container.begin(); it != container.end(); ++it, ++counter) {

						out.Write(L"item", Pair<T::key_type, T::mapped_type>(it->first, it->second));
						//out.Write((L"item_second_"+StringUtil::To<std::wstring>(counter)).c_str(), it->second);
					}
				}
				virtual void Deserialize(IStreamReader&) { assert(false); }
			};

			template <typename T>
			class ContainerDeserialize : public ISerializable {
				T& container;
			public:
				ContainerDeserialize<T>(T& pContainer) : container(pContainer) {}

				virtual void Serialize(IStreamWriter&) const { assert(false); }
				virtual void Deserialize(IStreamReader& in) {
					in.Begin(L"array");
					size_t size = in.GetCount(L"item");

					for (size_t i = 0; i < size; ++i) {
						typename T::value_type value;
						in.Read(L"item", value);
						container.push_back(value);
					}
				}
			};

			template <typename T>
			class SetContainerDeserialize : public ISerializable {
				T& container;
			public:
				SetContainerDeserialize<T>(T& pContainer) : container(pContainer) {}

				virtual void Serialize(IStreamWriter&) const { assert(false); }
				virtual void Deserialize(IStreamReader& in) {
					in.Begin(L"array");
					size_t size = in.GetCount(L"item");

					for (size_t i = 0; i < size; ++i) {
						typename T::value_type value;
						in.Read(L"item", value);
						container.insert(value);
					}
				}
			};

			template <typename T>
			class KeyedContainerDeserialize : public ISerializable {
				T& container;
			public:
				KeyedContainerDeserialize<T>(T& pContainer) : container(pContainer) {}

				virtual void Serialize(IStreamWriter&) const { assert(false); }
				virtual void Deserialize(IStreamReader& in) {
					in.Begin(L"associative_array");
					size_t size = in.GetCount(L"item");
					//in.Read(L"size", size);

					for (size_t i = 0; i < size; ++i) {
						Pair<T::key_type, T::mapped_type> pair;
						//std::pair<T::key_type, T::mapped_type> value;
						in.Read(L"item", pair);
						//in.Read((L"item_second_"+StringUtil::To<std::wstring>(i)).c_str(),value.second);
						container.insert(pair.Value);
					}

				}
			};

			template <typename T>
			struct WriterWrapper : ISerializable {
				T& Value;
				WriterWrapper<T>(T& value) : Value(value) {}
				virtual void Serialize(Serializer::IStreamWriter& out) const { Value.Serialize(out); }
				virtual void Deserialize(Serializer::IStreamReader&) { assert(false); }
			};

			template <typename T>
			struct ReaderWrapper : ISerializable {
				T& Value;
				ReaderWrapper<T>(T& value) : Value(value) {}
				virtual void Serialize(Serializer::IStreamWriter&) const { assert(false); }
				virtual void Deserialize(Serializer::IStreamReader& in) { Value.Deserialize(in); }
			};

			template <typename T>
			struct Matrix4x4Serialize : ISerializable {
				const T& m;

				Matrix4x4Serialize(const T& M) :m(M) {}
				virtual void Serialize(Serializer::IStreamWriter& out) const {
					out.Begin(L"float4x4");
					out.Write(L"Row1", m[0]);
					out.Write(L"Row2", m[1]);
					out.Write(L"Row3", m[2]);
					out.Write(L"Row4", m[3]);
				}
				virtual void Deserialize(Serializer::IStreamReader&) { assert(false); }
			};

			template <typename T>
			struct Matrix4x4Deserialize : ISerializable {
				T& m;

				Matrix4x4Deserialize(T& M) :m(M) {}
				virtual void Serialize(Serializer::IStreamWriter&) const {
					assert(false);
				}
				virtual void Deserialize(Serializer::IStreamReader& in) {
					in.Begin(L"float4x4");
					in.Read(L"Row1", m[0]);
					in.Read(L"Row2", m[1]);
					in.Read(L"Row3", m[2]);
					in.Read(L"Row4", m[3]);
				}
			};

		} // namespace Private
		
		#pragma region Inline definitions of IStreamWriter
		template<typename T>
		inline void IStreamWriter::writeContainer(const wchar_t* name, const T& container)
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

		template<typename T>
		inline void IStreamWriter::writeKeyedContainer(const wchar_t* name, const T& container)
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

		template<int Size>
		inline void IStreamWriter::Write(const wchar_t* name, const static_string<Size>& value){ Write(name, value.c_str()); }

		template<int Size>
		inline void IStreamWriter::Write(const wchar_t* name, const static_wstring<Size>& value) { Write(name, value.c_str()); }

		template<typename T>
		inline void IStreamWriter::Write(const wchar_t* name, const T& value)
		{
			Private::WriterWrapper<const T> helper(value);
			Write(name, static_cast<const ISerializable&>(helper));
		}

		template<typename T>
		inline void IStreamWriter::WriteArray(const wchar_t* name, const T* array, unsigned int size)
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

		template<typename T>
		inline void IStreamWriter::Write(const wchar_t* name, const std::vector<T>& value)
		{
			writeContainer(name, value);
		}

		template<typename T>
		inline void IStreamWriter::Write(const wchar_t* name, const std::list<T>& value)
		{
			writeContainer(name, value);
		}

		template<typename T>
		inline void IStreamWriter::Write(const wchar_t* name, const std::deque<T>& value)
		{
			writeContainer(name, value);
		}

		template<typename T>
		inline void IStreamWriter::Write(const wchar_t* name, const std::set<T>& value)
		{
			writeContainer(name, value);
		}

		template <typename T1, typename T2>
		inline void IStreamWriter::Write(const wchar_t* name, const std::set<T1, T2>& value){ writeContainer(name, value); }

		template <typename T1, typename T2>
		inline void IStreamWriter::Write(const wchar_t* name, const std::map<T1, T2>& value) { writeKeyedContainer(name, value); }

		template <typename T1, typename T2, typename T3>
		inline void IStreamWriter::Write(const wchar_t* name, const std::map<T1, T2, T3>& value) { writeKeyedContainer(name, value); }

		template <typename T1, typename T2>
		inline void IStreamWriter::Write(const wchar_t* name, const std::pair<T1, T2>& value)
		{
			Private::Pair<T1, T2> helper(value.first, value.second);
			Write(name, /*static_cast<const ISerializable&>*/ (helper));
		}
		template<typename T>
		inline void IStreamWriter::WriteEnum(const wchar_t* name, const T& value)
		{
			uint32 tmp = static_cast<uint32>(value);
			Write(name, tmp);
		}
		
		#pragma endregion
		
		#pragma region Inline Definitions of IStreanReader
		template<typename T>
		inline void IStreamReader::readContainer(const wchar_t* name, T& container)
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
		inline void IStreamReader::readSetContainer(const wchar_t* name, T& container)
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
		inline void IStreamReader::readKeyedContainer(const wchar_t* name, T& container)
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
		
		template <int Size>
		inline void IStreamReader::Read(const wchar_t* name, static_wstring<Size>& value)
		{
			std::vector<wchar_t> buffer(value.max_size());
			Read(name, &buffer[0], (uint32)value.max_size());
			value = &buffer[0];
			//Read(name,value.c_str(),unsigned int(value.max_size()));
		}
		
		template <int Size>
		inline void IStreamReader::Read(const wchar_t* name, static_string<Size>& value)
		{
			std::vector<char> buffer(value.max_size());
			Read(name, &buffer[0], (uint32)value.max_size());
			value = &buffer[0];
			//Read(name,value.c_str(),unsigned int(value.max_size()));
		}

		template <typename T>
		inline void IStreamReader::Read(const wchar_t* name, T& value)
		{
			Private::ReaderWrapper<T> helper(value);
			Read(name, static_cast<ISerializable&>(helper));
		}

		template <typename T>
		inline void IStreamReader::ReadArray(const wchar_t* name, T* array, unsigned int size)
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
		inline void IStreamReader::Read(const wchar_t* name, std::vector<T>& value)
		{
			value.clear();
			readContainer(name, value);
		}
		
		template <typename T>
		inline void IStreamReader::Read(const wchar_t* name, std::list<T>& value)
		{
			value.clear();
			readContainer(name, value);
		}
		
		template <typename T>
		inline void IStreamReader::Read(const wchar_t* name, std::deque<T>& value)
		{
			value.clear();
			readContainer(name, value);
		}
		
		template <typename T>
		inline void IStreamReader::Read(const wchar_t* name, std::set<T>& value)
		{
			value.clear();
			readSetContainer(name, value);
		}
		
		template <typename T1, typename T2>
		inline void IStreamReader::Read(const wchar_t* name, std::set<T1, T2>& value)
		{
			value.clear();
			readSetContainer(name, value);
		}

		template <typename T1, typename T2>
		inline void IStreamReader::Read(const wchar_t* name, std::map<T1, T2>& value)
		{
			value.clear();
			readKeyedContainer(name, value);
		}
		
		template <typename T1, typename T2, typename T3>
		inline void IStreamReader::Read(const wchar_t* name, std::map<T1, T2, T3>& value)
		{
			value.clear();
			readKeyedContainer(name, value);
		}
		
		template <typename T1, typename T2, typename T3>
		inline void IStreamReader::Read(const wchar_t* name, std::map<T1, T2, T3>& value, bool clear)
		{
			if (clear) {
				value.clear();
			}
			readKeyedContainer(name, value);
		}
		
		template <typename T1, typename T2>
		inline void IStreamReader::Read(const wchar_t* name, std::pair<T1, T2>& value)
		{
			//value.clear();
			Private::Pair<T1, T2> helper;
			Read(name, helper);

			value = helper.Value;
		}

		template <typename T>
		inline void IStreamReader::Read(const wchar_t* name, T& value, const T& defaultValue)
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
		inline void IStreamReader::ReadEnum(const wchar_t* name, T& value, const T& defaultValue)
		{
			uint32 tmp;
			Read(name, tmp, uint32(defaultValue));
			value = static_cast<T>(tmp);
		}

		template <typename T>
		inline void IStreamReader::ReadEnum(const wchar_t* name, T& value)
		{
			uint32 tmp;
			Read(name, tmp);
			value = static_cast<T>(tmp);
		}
#pragma endregion	
		
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
    inline const wchar_t *GetTypeName() { return TypeName; }
#endif