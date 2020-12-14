#include "Loader.h"

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				void FilePosData::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"XRated::Database::Info::FilePosData");
					out.Write(L"StartPos", StartPos);
					out.Write(L"EndPos", EndPos);
				}
				void FilePosData::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"XRated::Database::Info::FilePosData");
					in.Read(L"StartPos", StartPos);
					in.Read(L"EndPos", EndPos);
				}
				// LoaderManager Start ////////////////////////////////////////////////////

				LoaderManager::LoaderManager()
					: fileOfEnd(0), fileReader(NULL)
				{
				}

				LoaderManager::~LoaderManager()
				{
					fileReader = NULL;
				}

				void LoaderManager::Init(const String& fileName)
				{
					String fullFilename = Resource::ResourceSystemInstance().GetFullName(fileName.c_str());

					if ((fileReader == NULL) || (fullFilename != fileReader->GetName())) {
						fileReader = NULL;
						fileReader = Resource::ResourceSystemInstance().CreateStreamReader(fullFilename.c_str());
					}
				}

				template <typename T>
				void LoaderManager::Init(const String& filename, T& data)
				{
					Init(filename);

					fileReader->SetReadCursor(fileOfEnd, IStream::CursorPosition::Begin);

					Resource::SerializerStreamReader readStream = Serializer::CreateBinaryStreamReader(fileReader);
					uint32 size;
					readStream->Read(L"size", size);

					uint32 readhash;

					for (uint32 i = 0; i < size; ++i)
					{
						readStream->Read(L"header", readhash);
						dataPointer[readhash].StartPos = fileReader->GetReadCursor();

						data.Deserialize(*readStream);

						dataPointer[readhash].EndPos = fileReader->GetReadCursor();
					}

					fileOfEnd = fileReader->GetReadCursor();
				}

				template <typename T>
				bool LoaderManager::Retrieve(uint32 hash, T& result)
				{
					if (hash == 0) return false;

					if (dataPointer.find(hash) == dataPointer.end())
						return false;

					fileReader->SetReadCursor(dataPointer[hash].StartPos, IStream::CursorPosition::Begin);

					Resource::SerializerStreamReader readStream = Serializer::CreateBinaryStreamReader(fileReader);

					result.Deserialize(*readStream);

					return true;
				}

				template <typename T> void LoaderManager::Read(const wchar_t* name, T& result)
				{
					fileReader->SetReadCursor(fileOfEnd, IStream::CursorPosition::Begin);

					Resource::SerializerStreamReader readStream = Serializer::CreateBinaryStreamReader(fileReader);
					readStream->Read(name, result);

					fileOfEnd = fileReader->GetReadCursor();
				}

				void LoaderManager::Load(const std::wstring& file)
				{
					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateSerializerStructuredBinaryStreamReader(file.c_str());
					reader->Read(L"IndexedManager", dataPointer);
				}
				void LoaderManager::Save(const std::wstring& file)
				{
					Resource::SerializerStreamWriter writer = Resource::ResourceSystemInstance().CreateSerializerStructuredBinaryStreamWriter(file.c_str());
					writer->Write(L"IndexedManager", dataPointer);
				}
				// LoaderManager End ////////////////////////////////////////////////////
				// Loader Start ////////////////////////////////////////////////////
				Loader::Loader(const wchar_t* file)
					:fileOfEnd(0), fileReader(NULL)
				{
					fileReader = Resource::ResourceSystemInstance().CreateStreamReader(file);
				}
				Loader::~Loader()
				{
					fileReader = NULL;
				}

				template <typename T>
				void Loader::MakeIndex(std::map<uint32, FilePosData>& table, T& data)
				{
					fileReader->SetReadCursor(fileOfEnd, IStream::CursorPosition::Begin);

					Resource::SerializerStreamReader readStream = Serializer::CreateBinaryStreamReader(fileReader);

					uint32 size;
					readStream->Read(L"size", size);

					uint32 readhash;

					for (uint32 i = 0; i < size; ++i)
					{
						readStream->Read(L"header", readhash);
						table[readhash].StartPos = fileReader->GetReadCursor();

						data.Deserialize(*readStream);

						table[readhash].EndPos = fileReader->GetReadCursor();
					}

					fileOfEnd = fileReader->GetReadCursor();
				}
				template <typename T>
				void Loader::MakeIndex(FilePosData& filePositon, const std::wstring& element, T& data)
				{
					fileReader->SetReadCursor(fileOfEnd, IStream::CursorPosition::Begin);

					filePositon.StartPos = fileReader->GetReadCursor();

					Resource::SerializerStreamReader readStream = Serializer::CreateBinaryStreamReader(fileReader);

					readStream->Read(element.c_str(), data);

					filePositon.EndPos = fileReader->GetReadCursor();

					fileOfEnd = fileReader->GetReadCursor();
				}


				template <typename T>
				void Loader::Read(const FilePosData& filePosition, T& result)
				{
					fileReader->SetReadCursor(filePosition.StartPos, IStream::CursorPosition::Begin);

					Resource::SerializerStreamReader readStream = Serializer::CreateBinaryStreamReader(fileReader);

					result.Deserialize(*readStream);
				}

				template <typename T>
				void Loader::Read(const FilePosData& filePosition, const std::wstring& element, T& result)
				{
					fileReader->SetReadCursor(filePosition.StartPos, IStream::CursorPosition::Begin);

					Resource::SerializerStreamReader readStream = Serializer::CreateBinaryStreamReader(fileReader);

					readStream->Read(element.c_str(), result);
				}
				com_ptr<Loader> CreateLoader(const wchar_t* file)
				{
					return new Loader(file);
				}
				// Loader End ////////////////////////////////////////////////////

				// IndexedManager Start ////////////////////////////////////////////////////
				template <typename T>
				IndexedManagerWithMap::IndexedManagerWithMap(com_ptr<Loader> ptr, T& data)
					: loader(ptr)
				{
					loader->MakeIndex(dataPointer, data);
				}
				IndexedManagerWithMap::IndexedManagerWithMap(com_ptr<Loader> ptr)
					: loader(ptr)
				{
				}
				IndexedManagerWithMap::~IndexedManagerWithMap()
				{
					loader = NULL;
				}


				template <typename T>
				bool IndexedManagerWithMap::Get(uint32 hash, T& result)
				{
					DataPointerIter iter = dataPointer.find(hash);
					if (iter != dataPointer.end()) {
						loader->Read(iter->second, result);
						return true;
					}
					return false;
				}
				void IndexedManagerWithMap::Load(const std::wstring& file)
				{
					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateSerializerStructuredBinaryStreamReader(file.c_str());
					reader->Read(L"IndexedManagerWithMap", dataPointer);
				}
				void IndexedManagerWithMap::Save(const std::wstring& file)
				{
					Resource::SerializerStreamWriter writer = Resource::ResourceSystemInstance().CreateSerializerStructuredBinaryStreamWriter(file.c_str());
					writer->Write(L"IndexedManagerWithMap", dataPointer);
				}
				template <typename T>
				com_ptr<IndexedManagerWithMap> CreateIndexedManagerWithMap(com_ptr<Loader> ptr, T& data)
				{
					return new IndexedManagerWithMap(ptr, data);
				}
				com_ptr<IndexedManagerWithMap> CreateIndexedManagerWithMap(com_ptr<Loader> ptr)
				{
					return new IndexedManagerWithMap(ptr);
				}
				// IndexedManager End ////////////////////////////////////////////////////

				// IndexedManagerWithElement Start ////////////////////////////////////////////////////
				template <typename T>
				IndexedManagerWithElement::IndexedManagerWithElement(com_ptr<Loader> ptr, const std::wstring& name, T& data)
					:element(name), loader(ptr)
				{
					loader->MakeIndex(filePosition, name, data);
				}
				IndexedManagerWithElement::IndexedManagerWithElement(com_ptr<Loader> ptr, const std::wstring& name)
					: element(name), loader(ptr)
				{
				}
				IndexedManagerWithElement::~IndexedManagerWithElement()
				{
					loader = NULL;
				}
				template <typename T>
				bool IndexedManagerWithElement::Get(T& result)
				{
					loader->Read(filePosition, element, result);
					return true;
				}
				void IndexedManagerWithElement::Save(const std::wstring& file)
				{
					Resource::SerializerStreamWriter writer = Resource::ResourceSystemInstance().CreateSerializerStructuredBinaryStreamWriter(file.c_str());
					writer->Write(L"IndexedManagerWithElement", filePosition);
				}
				void IndexedManagerWithElement::Load(const std::wstring& file)
				{
					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateSerializerStructuredBinaryStreamReader(file.c_str());
					reader->Read(L"IndexedManagerWithElement", filePosition);
				}

				template <typename T> com_ptr<IndexedManagerWithElement> CreateIndexedManagerWithElement(com_ptr<Loader> ptr, const std::wstring& element, T& data)
				{
					return new IndexedManagerWithElement(ptr, element, data);
				}
				com_ptr<IndexedManagerWithElement> CreateIndexedManagerWithElement(com_ptr<Loader> ptr, const std::wstring& element)
				{
					return new IndexedManagerWithElement(ptr, element);
				}
				// IndexedManagerWithElement End ////////////////////////////////////////////////////
			}
		}
	}
}