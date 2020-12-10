#ifndef PrivateFlat_H
#define PrivateFlat_H

#pragma once
#include <Core/Serializer/Serializer.h>
		namespace PrivateFlat {

			template <typename T> class ArraySerialize : public ISerializable {
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

			template <typename T> class ArrayDeserialize : public ISerializable {
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
							Logger::GetInstance()->Warn("static array being deserialized has bigger size on file");
					}
					else {
						Logger::GetInstance()->Exception(L"static array being deserialized has smaller size on file");
					}

					for (unsigned int i = 0; i < size; ++i) {
						T value;
						in.Read((L"item_" + StringUtil::To<std::wstring>(i)).c_str(), value);
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
					out.Write(L"size", unsigned int(container.size()));
					int counter = 0;
					for (auto it = container.begin(); it != container.end(); ++it, ++counter) {
						out.Write((L"item_" + StringUtil::To<std::wstring>(counter)).c_str(), *it);
					}
				}
				virtual void Deserialize(IStreamReader&) { assert(false); }
			};

			template <typename T> class KeyedContainerSerialize : public ISerializable {
				const T& container;
			public:
				KeyedContainerSerialize<T>(const T& pContainer) : container(pContainer) {}

				virtual void Serialize(IStreamWriter& out) const {
					out.Begin(L"associative_array");
					out.Write(L"size", unsigned int(container.size()));
					int counter = 0;
					for (auto it = container.begin(); it != container.end(); ++it, ++counter) {
						out.Write((L"item_first_" + StringUtil::To<std::wstring>(counter)).c_str(), it->first);
						out.Write((L"item_second_" + StringUtil::To<std::wstring>(counter)).c_str(), it->second);
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
					unsigned int size;
					in.Read(L"size", size);

					for (unsigned int i = 0; i < size; ++i) {
						typename T::value_type value;
						in.Read((L"item_" + StringUtil::To<std::wstring>(i)).c_str(), value);
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
					unsigned int size;
					in.Read(L"size", size);

					for (unsigned int i = 0; i < size; ++i) {
						typename T::value_type value;
						in.Read((L"item_" + StringUtil::To<std::wstring>(i)).c_str(), value);
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

#endif // !PrivateFlat_H