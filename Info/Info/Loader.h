#pragma once

#include "Database.h"

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {

				struct FilePosData : public Serializer::ISerializable
				{
					uint32 StartPos;
					uint32 EndPos;

					virtual void Serialize(Serializer::IStreamWriter& out) const;
					virtual void Deserialize(Serializer::IStreamReader& in);
				};

				class LoaderManager
				{
				protected:
					com_ptr<IRefCountedStreamReader> fileReader;

					std::map<uint32, FilePosData> dataPointer;

					uint32 fileOfEnd;
				public:
					void Load(const std::wstring& file);
					void Save(const std::wstring& file);
				public:
					LoaderManager();
					~LoaderManager();

					void Init(const String& fileName);
					template <typename T> void Init(const String& filename, T& data);
					template <typename T> void Read(const wchar_t* name, T& data);
					template <typename T> bool Retrieve(uint32 hash, T& result);
				};
				class IndexedManagerWithMap;

				class Loader : public ReferenceCounted
				{
					com_ptr<IRefCountedStreamReader> fileReader;
					uint32 fileOfEnd;
				public:
					template <typename T> void MakeIndex(std::map<uint32, FilePosData>& table, T& data);
					template <typename T> void MakeIndex(FilePosData& filePositon, const std::wstring& element, T& data);
					template <typename T> void Read(const FilePosData& filePosition, T& result);
					template <typename T> void Read(const FilePosData& filePosition, const std::wstring& element, T& result);
				public:
					Loader(const wchar_t* file);
					~Loader();
				};

				com_ptr<Loader> CreateLoader(const wchar_t* file);

				class IndexedManagerWithMap : public ReferenceCounted
				{
				protected:
					std::map<uint32, FilePosData> dataPointer;
					typedef std::map<uint32, FilePosData>::iterator DataPointerIter;
					typedef std::map<uint32, FilePosData>::const_iterator DataPointerConstIter;
					com_ptr<Loader>		loader;
				public:
					template <typename T> bool Get(uint32 hash, T& result);

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
					template <typename T> IndexedManagerWithMap(com_ptr<Loader> ptr, T& data);
					IndexedManagerWithMap(com_ptr<Loader> ptr);
					~IndexedManagerWithMap();
				};

				template <typename T> com_ptr<IndexedManagerWithMap> CreateIndexedManagerWithMap(com_ptr<Loader> ptr, T& data);
				com_ptr<IndexedManagerWithMap> CreateIndexedManagerWithMap(com_ptr<Loader> ptr);

				class IndexedManagerWithElement : public ReferenceCounted
				{
					const std::wstring		element;
					FilePosData	filePosition;
					com_ptr<Loader>		loader;
				public:
					void Save(const std::wstring& file);
					void Load(const std::wstring& file);
				public:
					template <typename T> bool Get(T& result);
				public:
					template <typename T> IndexedManagerWithElement(com_ptr<Loader> ptr, const std::wstring& name, T& data);
					IndexedManagerWithElement(com_ptr<Loader> ptr, const std::wstring& name);
					~IndexedManagerWithElement();
				};

				template <typename T> com_ptr<IndexedManagerWithElement> CreateIndexedManagerWithElement(com_ptr<Loader> ptr, const std::wstring& element, T& data);
				com_ptr<IndexedManagerWithElement> CreateIndexedManagerWithElement(com_ptr<Loader> ptr, const std::wstring& element);
			}
		}
	}
}