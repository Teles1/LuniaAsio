#pragma once
#include <Info/Info/IndexedInfoManager.h>

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {

				void FilePosData::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::FilePosData");
					out.Write(L"StartPos", StartPos);
					out.Write(L"EndPos", EndPos);
				}
				void FilePosData::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::FilePosData");
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

				
				

				void LoaderManager::Load(const std::wstring& file)
				{
					Resource::SerializerStreamReader reader = Lunia::Resource::ResourceSystemInstance().CreateSerializerStructuredBinaryStreamReader(file.c_str());
					reader->Read(L"IndexedManager", dataPointer);
				}
				void LoaderManager::Save(const std::wstring& file)
				{
					Resource::SerializerStreamWriter writer = Lunia::Resource::ResourceSystemInstance().CreateSerializerStructuredBinaryStreamWriter(file.c_str());
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

				
				com_ptr<Loader> CreateLoader(const wchar_t* file)
				{
					return new Loader(file);
				}
				// Loader End ////////////////////////////////////////////////////

				// IndexedManager Start ////////////////////////////////////////////////////
				
				IndexedManagerWithMap::IndexedManagerWithMap(com_ptr<Loader> ptr)
					: loader(ptr)
				{
				}
				IndexedManagerWithMap::~IndexedManagerWithMap()
				{
					loader = NULL;
				}


				
				void IndexedManagerWithMap::Load(const std::wstring& file)
				{
					Resource::SerializerStreamReader reader = Lunia::Resource::ResourceSystemInstance().CreateSerializerStructuredBinaryStreamReader(file.c_str());
					reader->Read(L"IndexedManagerWithMap", dataPointer);
				}
				void IndexedManagerWithMap::Save(const std::wstring& file)
				{
					Resource::SerializerStreamWriter writer = Lunia::Resource::ResourceSystemInstance().CreateSerializerStructuredBinaryStreamWriter(file.c_str());
					writer->Write(L"IndexedManagerWithMap", dataPointer);
				}
				
				com_ptr<IndexedManagerWithMap> CreateIndexedManagerWithMap(com_ptr<Loader> ptr)
				{
					return new IndexedManagerWithMap(ptr);
				}
				// IndexedManager End ////////////////////////////////////////////////////

				// IndexedManagerWithElement Start ////////////////////////////////////////////////////
				
				IndexedManagerWithElement::IndexedManagerWithElement(com_ptr<Loader> ptr, const std::wstring& name)
					: element(name), loader(ptr)
				{
				}
				IndexedManagerWithElement::~IndexedManagerWithElement()
				{
					loader = NULL;
				}
				
				void IndexedManagerWithElement::Save(const std::wstring& file)
				{
					Resource::SerializerStreamWriter writer = Lunia::Resource::ResourceSystemInstance().CreateSerializerStructuredBinaryStreamWriter(file.c_str());
					writer->Write(L"IndexedManagerWithElement", filePosition);
				}
				void IndexedManagerWithElement::Load(const std::wstring& file)
				{
					Resource::SerializerStreamReader reader = Lunia::Resource::ResourceSystemInstance().CreateSerializerStructuredBinaryStreamReader(file.c_str());
					reader->Read(L"IndexedManagerWithElement", filePosition);
				}

				
				com_ptr<IndexedManagerWithElement> CreateIndexedManagerWithElement(com_ptr<Loader> ptr, const std::wstring& element)
				{
					return new IndexedManagerWithElement(ptr, element);
				}
			}
		}
	}
}