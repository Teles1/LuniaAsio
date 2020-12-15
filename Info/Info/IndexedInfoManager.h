#pragma once
#include <Core/FileIO/FileStream.h>
#include <Core/Serializer/Serializer.h>
#include <Core/Resource/Resource.h>
#include <Core/Serializer/Binary/BinaryStream.h>

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {

				struct FilePosData : public Serializer::ISerializable
				{
					Lunia::uint32 StartPos;
					Lunia::uint32 EndPos;

					virtual void Serialize(Serializer::IStreamWriter& out) const;
					virtual void Deserialize(Serializer::IStreamReader& in);
				};

				class LoaderManager
				{
				protected:
					com_ptr<IRefCountedStreamReader> fileReader;

					std::map<Lunia::uint32, FilePosData> dataPointer;

					Lunia::uint32 fileOfEnd;
				public:
					void Load(const std::wstring& file);
					void Save(const std::wstring& file);
				public:
					LoaderManager();
					~LoaderManager();

					void Init(const String& fileName);
					template <typename T>
					inline void Init(const String& filename, T& data)
					{
						Init(filename);

						fileReader->SetReadCursor(fileOfEnd, Lunia::IStream::CursorPosition::Begin);

						Lunia::Resource::SerializerStreamReader readStream = Serializer::CreateBinaryStreamReader(fileReader);

						Lunia::uint32 size;
						readStream->Read(L"size", size);

						Lunia::uint32 readhash;

						for (Lunia::uint32 i = 0; i < size; ++i)
						{
							readStream->Read(L"header", readhash);
							dataPointer[readhash].StartPos = fileReader->GetReadCursor();

							data.Deserialize(*readStream);

							dataPointer[readhash].EndPos = fileReader->GetReadCursor();
						}

						fileOfEnd = fileReader->GetReadCursor();
					}


					template <typename T>
					inline bool Retrieve(Lunia::uint32 hash, T& result)
					{
						if (hash == 0) return false;

						if (dataPointer.find(hash) == dataPointer.end())
							return false;

						fileReader->SetReadCursor(dataPointer[hash].StartPos, Lunia::IStream::CursorPosition::Begin);

						Lunia::Resource::SerializerStreamReader readStream = Serializer::CreateBinaryStreamReader(fileReader);

						result.Deserialize(*readStream);

						return true;
					}

					template <typename T> 
					inline void Read(const wchar_t* name, T& result)
					{
						fileReader->SetReadCursor(fileOfEnd, Lunia::IStream::CursorPosition::Begin);

						Lunia::Resource::SerializerStreamReader readStream = Serializer::CreateBinaryStreamReader(fileReader);

						readStream->Read(name, result);

						fileOfEnd = fileReader->GetReadCursor();
					}
				};

				class Loader : public Lunia::ReferenceCounted
				{
					com_ptr<IRefCountedStreamReader> fileReader;
					Lunia::uint32 fileOfEnd;
			
				public:
					template <typename T>
					inline void MakeIndex(std::map<Lunia::uint32, FilePosData>& table, T& data)
					{
						fileReader->SetReadCursor(fileOfEnd, Lunia::IStream::CursorPosition::Begin);

						Lunia::Resource::SerializerStreamReader readStream = Serializer::CreateBinaryStreamReader(fileReader);

						Lunia::uint32 size;
						readStream->Read(L"size", size);

						Lunia::uint32 readhash;

						for (Lunia::uint32 i = 0; i < size; ++i)
						{
							readStream->Read(L"header", readhash);
							table[readhash].StartPos = fileReader->GetReadCursor();

							data.Deserialize(*readStream);

							table[readhash].EndPos = fileReader->GetReadCursor();
						}

						fileOfEnd = fileReader->GetReadCursor();
					}
					template <typename T>
					inline void MakeIndex(FilePosData& filePositon, const std::wstring& element, T& data)
					{
						fileReader->SetReadCursor(fileOfEnd, Lunia::IStream::CursorPosition::Begin);

						filePositon.StartPos = fileReader->GetReadCursor();

						Lunia::Resource::SerializerStreamReader readStream = Serializer::CreateBinaryStreamReader(fileReader);

						readStream->Read(element.c_str(), data);

						filePositon.EndPos = fileReader->GetReadCursor();

						fileOfEnd = fileReader->GetReadCursor();
					}


					template <typename T>
					inline void Read(const FilePosData& filePosition, T& result)
					{
						fileReader->SetReadCursor(filePosition.StartPos, Lunia::IStream::CursorPosition::Begin);

						Lunia::Resource::SerializerStreamReader readStream = Serializer::CreateBinaryStreamReader(fileReader);

						result.Deserialize(*readStream);
					}

					template <typename T>
					inline void Read(const FilePosData& filePosition, const std::wstring& element, T& result)
					{
						fileReader->SetReadCursor(filePosition.StartPos, Lunia::IStream::CursorPosition::Begin);

						Lunia::Resource::SerializerStreamReader readStream = Serializer::CreateBinaryStreamReader(fileReader);

						readStream->Read(element.c_str(), result);
					}
				public:
					Loader(const wchar_t* file);
					~Loader();
				};

				com_ptr<Loader> CreateLoader(const wchar_t* file);

				class IndexedManagerWithMap : public Lunia::ReferenceCounted
				{
				protected:
					std::map<Lunia::uint32, FilePosData> dataPointer;
					typedef std::map<Lunia::uint32, FilePosData>::iterator DataPointerIter;
					typedef std::map<Lunia::uint32, FilePosData>::const_iterator DataPointerConstIter;
					com_ptr<Loader>		loader;
				public:
					template <typename T>
					inline bool Get(Lunia::uint32 hash, T& result)
					{
						DataPointerIter iter = dataPointer.find(hash);
						if (iter != dataPointer.end()) {
							loader->Read(iter->second, result);
							return true;
						}
						return false;
					}

				public:
					void Save(const std::wstring& file);
					void Load(const std::wstring& file);

					void GetHashs(std::vector<uint32>& vt) const
					{
						vt.clear();
						DataPointerConstIter it = dataPointer.begin();
						for (; it != dataPointer.end(); ++it)
							vt.push_back(it->first);
					}
				public:
					template <typename T>
					inline IndexedManagerWithMap(com_ptr<Loader> ptr, T& data)
						: loader(ptr)
					{
						loader->MakeIndex(dataPointer, data);
					}
					IndexedManagerWithMap(com_ptr<Loader> ptr);
					~IndexedManagerWithMap();
				};

				template <typename T>
				inline com_ptr<IndexedManagerWithMap> CreateIndexedManagerWithMap(com_ptr<Loader> ptr, T& data)
				{
					return new IndexedManagerWithMap(ptr, data);
				}
				com_ptr<IndexedManagerWithMap> CreateIndexedManagerWithMap(com_ptr<Loader> ptr);
				class IndexedManagerWithElement : public Lunia::ReferenceCounted
				{
					const std::wstring		element;
					FilePosData	filePosition;
					com_ptr<Loader>		loader;
				public:
					void Save(const std::wstring& file);
					void Load(const std::wstring& file);
				public:
					template <typename T>
					inline bool Get(T& result)
					{
						loader->Read(filePosition, element, result);
						return true;
					}
				public:
					template <typename T>
					inline IndexedManagerWithElement(com_ptr<Loader> ptr, const std::wstring& name, T& data)
						:element(name), loader(ptr)
					{
						loader->MakeIndex(filePosition, name, data);
					}
					IndexedManagerWithElement(com_ptr<Loader> ptr, const std::wstring& name);
					~IndexedManagerWithElement();
				};

				template <typename T> 
				inline com_ptr<IndexedManagerWithElement> CreateIndexedManagerWithElement(com_ptr<Loader> ptr, const std::wstring& element, T& data)
				{
					return new IndexedManagerWithElement(ptr, element, data);
				}
				com_ptr<IndexedManagerWithElement> CreateIndexedManagerWithElement(com_ptr<Loader> ptr, const std::wstring& element);

			}
		}
	}
}