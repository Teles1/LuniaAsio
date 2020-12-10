#ifndef Private_H
#define Private_H
#pragma once
#include <Core/Serializer/Serializer.h>
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
					Logger::GetInstance()->Warn("static array being deserialized has bigger size on file");
			}
			else {
				Logger::GetInstance()->Exception("static array being deserialized has smaller size on file");
			}

			for (unsigned int i = 0; i < size; ++i) {
				T value;
				in.Read(L"item", value);
				buffer[i] = value;
			}
		}

	};
	/**
	@brief Helper classes for serializing std compliant containers
	*/

	template <typename T> class ContainerSerialize : public ISerializable {
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

	template <typename T1, typename T2> class Pair : public ISerializable {
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

	template <typename T> class KeyedContainerSerialize : public ISerializable {
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

	template <typename T> class ContainerDeserialize : public ISerializable {
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

	template <typename T> class SetContainerDeserialize : public ISerializable {
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

	template <typename T> class KeyedContainerDeserialize : public ISerializable {
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

	template <typename T> struct WriterWrapper : ISerializable {
		T& Value;
		WriterWrapper<T>(T& value) : Value(value) {}
		virtual void Serialize(Serializer::IStreamWriter& out) const { Value.Serialize(out); }
		virtual void Deserialize(Serializer::IStreamReader&) { assert(false); }
	};

	template <typename T> struct ReaderWrapper : ISerializable {
		T& Value;
		ReaderWrapper<T>(T& value) : Value(value) {}
		virtual void Serialize(Serializer::IStreamWriter&) const { assert(false); }
		virtual void Deserialize(Serializer::IStreamReader& in) { Value.Deserialize(in); }
	};

	template <typename T> struct Matrix4x4Serialize : ISerializable {
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

	template <typename T> struct Matrix4x4Deserialize : ISerializable {
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

#endif // !Private_H

