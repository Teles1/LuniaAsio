#include "Resource.h"
#include <Core/Serializer/Binary/XmlStream.h>
#include <Core/Serializer/Binary/BinaryStream.h>
#include <Core/Serializer/Binary/StructuredBinaryStream.h>
#include <Core/ReferenceCounted.h>
#include <Core/FileIO/FileStream.h>
#include "TextLoader.h"
#include "ChunkLoader.h"
#include "ConstantManager2.h"
#include "CacheController.h"
#include <Core/Utils/StringUtil/Compare.h>
#include <cctype>
#include <iostream>

namespace Lunia {
	namespace Resource {
		class Resource : public IResource {
		public:
			static Resource& GetInstance() {
				static Resource m_Instance;
				return m_Instance;
			};
			typedef std::map<std::wstring, std::wstring> Extensions;
			typedef std::map<std::wstring, std::wstring> ConsolidatedName;
			Extensions extensions;
			mutable ConsolidatedName consolidatedNames;
			CacheController cacheController;
			std::vector< std::wstring > paths;
			std::wstring startupPath;
			std::wstring binaryExtension;
			std::vector<IEventListener*> listeners;
			typedef std::map<std::wstring, IStreamFactory*> StreamFactories;
			StreamFactories streamFactories;

			struct FileStreamFactory : IResource::IStreamFactory {
				Resource& outer;

				FileStreamFactory(Resource& pOuter) :outer(pOuter) {}


				/**
				1. We check whether zip acceleration tables have the entry.
				2. We check the normal file system
				*/
				virtual StreamReader CreateStreamReader(const wchar_t* name)
				{
					//name is assumed to be consolidated.
					//if (outer.useZip && ZipInstance().Exists(name))
					//{
					//	//assumes that the FileStreamFactory::Exists() has been called before for this, and it always should have been
					//	//return ZipInstance().CreateZipStreamReader(outer.OpenStream(ZipInstance().GetZipFilename(name)),name);
					//	return ZipInstance().CreateZipStreamReader(name);
					//}
					//else 
					if (FileIO::File::Exists(name))
					{
						return new FileIO::RefCountedFileStreamReader(name);
					}
					else
					{
						wchar_t relativeName[4096];
						outer.GetRelativeName(name, relativeName, 4096);
						throw ResourceNotFoundException(L"Cannot find resource from file system or internal zip acceleration tables: {0}", name);
					}
				}

				virtual StreamReader CreateStreamReader(uint8* bufferIn, uint32 bufferSz)
				{
					return new FileIO::RefCountedMemoryStreamReader(bufferIn, bufferSz);
				}

				virtual StreamWriter CreateStreamWriter(const wchar_t* name)
				{
					return new FileIO::RefCountedFileStreamWriter(name);
				}

				virtual bool             Exists(const wchar_t* name) const {

					////does the file exists in the zip system
					//if (ZipInstance().Exists(name))
					//{
					//	return true;
					//}

					//does the file exists on the disk
					if (FileIO::File::Exists(name))
					{
						return true;
					}

					//checks if the zip for the path exists in the system already
					/*					if (ZipInstance().IsAlreadyTraversed(name)) {
					return false;
					}*/

					//starts traversing upwards in the directory hiearchy and trying to find .zip replacing a directory tree

					int basePathIndex = -1;
					std::wstring basePath;

					std::wstring lowername = name;

					for (size_t i = outer.paths.size(); i > 0; --i) {
						std::wstring path = outer.paths[i - 1];

						if (lowername.find(path) != std::wstring::npos) {
							basePathIndex = int(i - 1);

							basePath = outer.paths[basePathIndex];

							if (!basePath.empty()) {
								if (basePath[basePath.length() - 1] == L'/') {
									basePath = std::wstring(basePath.begin(), basePath.end() - 1); //we remove possible last /
								}
							}
							//							ALLM_IMPORTANT((L"found path where to stop =%s", path.c_str()));
							break;
						}
					}

					//try finding .zip that has the file by traversing upwards the directory tree
					std::wstring zipfilename = name;

					bool stopIteration = false;
					while ((!zipfilename.empty()) && (!stopIteration)) {
						Locator locator(zipfilename);
						if (basePathIndex != -1) {
							//we check if we have reached the base path and stop iteration
							if (StringUtil::EqualsCaseInsensitive(basePath, locator.GetPathNoFilename().c_str())) {
								//ALLM_IMPORTANT((L"stopping iterator, base path reached"));
								stopIteration = true;
								continue;
							}
						}

						zipfilename = locator.GetPathNoFilename().c_str();
						if (locator.GetFilename().empty()) return false;
						if (zipfilename.empty()) return false;

						zipfilename += L".zip";

						//		ALLM_INFO((L"trying to find zip filename=%s", zipfilename.c_str()));

						if (FileIO::File::Exists(zipfilename.c_str())) {
							//if (!ZipInstance().ZipExists(zipfilename.c_str())) {
							//	//ALLM_INFO((L"preloading zip"));
							//	ZipInstance().LoadZip(outer.OpenStream(zipfilename.c_str()));
							//	return ZipInstance().Exists(name);//we stop traversing when the first zip is found to keep things simple
							//}
							//else {
							return false;//we already checked existance for loaded zips.. first line of this method
						//}
						}
					}

					return false;
				}
			} fileStreamFactory;

			bool isAbsolute(const std::wstring& path) const {
				if (path.find(L":") != std::wstring::npos) {
					return true;
				}
				return false;
			}

			/**
			Helper function for the ConsolidatePath().

			@date 20061124
			@author juhnu
			*/
			void RemoveFromBuffer(wchar_t* buffer, size_t offset, size_t offset2, size_t maximumSize) const
			{
				assert(offset < offset2);

				size_t count = offset2 - offset;

				for (size_t i = offset; i + count < maximumSize; ++i)
				{
					buffer[i] = buffer[i + count];
				}
			}

			/**
			Consolidates the filename.

			@date 20061124
			@author juhnu

			1. Replaces all \ with /
			2. Changes to lowercase, except 'itemname' (part of path after '|' character)
			3. Removes ./ and ../

			@return true in case path is absolute, false if relative.
			*/
			bool ConsolidatePath(const wchar_t* name, wchar_t* buffer, size_t maximumSize) const
			{
				assert(maximumSize > 0);
				//make path absolute.
				bool absolute = false;
				//wcsncpy_s(buffer, name, maximumSize-1);
				for (size_t i = 0; i < maximumSize - 1; ++i)
				{
					buffer[i] = name[i];
					if (name[i] == L':') absolute = true; // c:/ d:/ etc..
					if (name[i] == 0) break;
				}
				buffer[maximumSize - 1] = 0;

				const int RecursionLimit = 32;//maximum recursion limit


				int32 slashPosition[RecursionLimit];//we could use explicit counter here, which probably would be faster, but as this works ok now I rather not touch it. --juhnu

				slashPosition[0] = 0;
				for (int i = 1; i < RecursionLimit; ++i)
				{
					slashPosition[i] = -1;
				}


				uint32 dotCounter = 0;
				bool prevSlash = true;

				for (size_t pos = 0; (pos < maximumSize) && (buffer[pos] != 0) && (buffer[pos] != L'|'); ++pos)
				{
					buffer[pos] = static_cast<wchar_t>(std::tolower(buffer[pos]));
					if (buffer[pos] == '\\') buffer[pos] = '/';
					if (buffer[pos] == '.')
					{
						if ((dotCounter != 0) || prevSlash)
						{
							++dotCounter;
						}
					}
					else if (buffer[pos] == '/')
					{
						if (dotCounter >= 2)
						{
							if (slashPosition[1] == -1) throw Exception(L"../ points over root level");
							//need to remove ../ and go back up one folder level
							RemoveFromBuffer(buffer, slashPosition[1], pos + 1, maximumSize);
							if (slashPosition[1] > 0)
							{
								pos = slashPosition[1] - 1;
							}
							else
							{
								pos = 0;
							}

							for (size_t i = 0; i < RecursionLimit - 1; ++i)
							{
								slashPosition[i] = slashPosition[i + 1];
							}

							slashPosition[RecursionLimit - 1] = -1;

							dotCounter = 0;
							prevSlash = true;

						}
						else if (dotCounter == 1)
						{
							if (slashPosition[0] == -1) throw Exception(L"./ points over root level");
							//we need to remove ./
							RemoveFromBuffer(buffer, slashPosition[0], pos + 1, maximumSize);
							if (slashPosition[0] > 0)
							{
								pos = slashPosition[0] - 1;
							}
							else
							{
								pos = 0;
							}

							dotCounter = 0;
							prevSlash = true;
						}
						else
						{
							for (size_t i = RecursionLimit - 1; i > 0; --i)
							{
								slashPosition[i] = slashPosition[i - 1];
							}
							slashPosition[0] = (int32)pos + 1;
							if (slashPosition[RecursionLimit - 1] != -1) throw Exception(L"recursion limit reached");

							prevSlash = true;
						}
					}
					else
					{
						prevSlash = false;
						dotCounter = 0;
					}

				}
				return absolute;
			}

		public:

			Resource() :
				fileStreamFactory(*this)
			{
				CurrentProgress = 0;
				MaximumProgress = 0;

				AddStreamFactory(L"file", &fileStreamFactory);

				startupPath = FileIO::Directory::GetCurrentDirectory() + L"/";
				AddPath(startupPath.c_str());
				//ALLM_INFO((L"startup directory is '%s'", startupPath.c_str()));
				//// read initial values
				//Config::ConfigInstance().Get(L"resource", L"binaryExtension", binaryExtension, L".sbf");
				//StringUtil::ToLowerCase(binaryExtension);

				//extensions[L".xml"] = binaryExtension;
				//extensions[L".png"] = L".dds";
				//extensions[L".jpg"] = L".dds";
				//extensions[L".bmp"] = L".dds";
				//extensions[L".tga"] = L".dds";
				//extensions[L".tif"] = L".dds";
				//extensions[L".fx"] = L".fxc";
			}

			virtual void AddEventListener(IEventListener* listener)
			{
				listeners.push_back(listener);
			}

			virtual void RemoveEventListener(IEventListener* listener)
			{
				for (size_t i = 0; i < listeners.size(); ++i)
				{
					if (listeners[i] == listener)
					{
						listeners[i] = listeners.back();
						listeners.pop_back();
						return;
					}
				}
			}

			virtual void AddStreamFactory(const wchar_t* prefix, IStreamFactory* listener)
			{
				streamFactories[prefix] = listener;
			}

			virtual void RemoveStreamFactory(IStreamFactory* listener)
			{
				for (StreamFactories::iterator i = streamFactories.begin(); i != streamFactories.end();) {
					if (i->second == listener) {
						i = streamFactories.erase(i);
						continue;
					}
					++i;
				}
				return;
			}

			virtual void RaiseCompleteEvent(const wchar_t* message)
			{
				for (size_t i = 0; i < listeners.size(); ++i)
				{
					listeners[i]->Completed(message);
				}
			}

			virtual void RaiseResourceChangedEvent(const wchar_t* resourceName)
			{
				for (size_t i = 0; i < listeners.size(); ++i)
				{
					listeners[i]->OnResourceChanged(resourceName);
				}
			}

			virtual void RaiseResourceOpenedEvent(const wchar_t* resourceName)
			{
				for (size_t i = 0; i < listeners.size(); ++i)
				{
					listeners[i]->OnResourceOpened(resourceName);
				}
			}

			virtual void GetConsolidatedName(const wchar_t* name, wchar_t* buffer, size_t maximumSize) const
			{
				ConsolidatedName::const_iterator iter = consolidatedNames.find(name);

				if (iter == consolidatedNames.end())
				{
					wchar_t temp[4096];
					GetConsolidatedName2(name, temp, 4096);

					consolidatedNames[name] = temp;
				}


				iter = consolidatedNames.find(name);
				for (size_t i = 0; i < maximumSize; ++i)
				{
					if (iter->second[i] == 0)
					{
						buffer[i] = 0;
						return;
					}

					buffer[i] = iter->second[i];
				}

				buffer[maximumSize - 1] = 0;

			}
			virtual void GetConsolidatedName2(const wchar_t* name, wchar_t* buffer, size_t maximumSize) const
			{
				Locator tmp(name);
				std::wstring ext = tmp.GetExtension().c_str();
				StringUtil::ToLowerCase(ext);
				Extensions::const_iterator it = extensions.find(ext);

				//			  ALLM_IMPORTANT((L"GetConsolidatedName() begin ---------------------------------------------------------------extension='%s'", ext.c_str()));

				if (it != extensions.end())
				{
					std::wstring primaryName = tmp.GetPathNoExtension().c_str();
					primaryName += it->second;

					static_wstring<512> item = tmp.GetItem();
					if (!item.empty())
					{
						primaryName += L"|";
						primaryName += item.c_str();
						GetConsolidatedNameImpl(primaryName.c_str(), buffer, maximumSize);

						int pipeChar = -1;
						for (size_t i = 0; i < maximumSize; ++i)
						{
							if (buffer[i] == 0) break;
							if (buffer[i] == L'|')
							{
								pipeChar = (int)i;
								buffer[i] = 0;
								break;
							}
						}

						if (pipeChar > 0) buffer[pipeChar] = 0;

						if (Exists(buffer)) {
							if (pipeChar > 0) buffer[pipeChar] = L'|';
							//						  ALLM_IMPORTANT((L"End ---------------------------------------primary extension with |-item"));
							return;
						}
					}
					else
					{
						GetConsolidatedNameImpl(primaryName.c_str(), buffer, maximumSize);
						if (Exists(buffer)) {
							//						  ALLM_IMPORTANT((L"End ---------------------------------------primary extension"));
							return;
						}
					}
				}

				GetConsolidatedNameImpl(name, buffer, maximumSize);
				//			  ALLM_IMPORTANT((L"End ---------------------------------------Normal"));
			}

			virtual void GetConsolidatedNameImpl(const wchar_t* target, wchar_t* buffer, size_t size) const
			{
				//			  ALLM_IMPORTANT((L"#0 CONSOLIDATED=%s", target))
				if (ConsolidatePath(target, buffer, size))
				{
					//					  ALLM_IMPORTANT((L"#1 CONSOLIDATED=%s", buffer))
					return;
				}
				else
				{
					int pipeChar = -1;
					for (size_t i = 0; i < size; ++i)
					{
						if (buffer[i] == 0)
						{
							break;
						}
						if (buffer[i] == L'|')
						{
							pipeChar = (int)i;
							buffer[i] = 0;
							break;
						}
					}
					if (pipeChar != 0)
					{
						for (size_t i = paths.size(); i > 0; --i) { //paths are quaranteed to be consolidated.
							std::wstring tmp2 = paths[i - 1] + buffer;
							//consolidated name quaranteed
							if (ExistsDispatch(tmp2)) {
								if (pipeChar >= 0)
								{
									buffer[pipeChar] = L'|';
									//we need to create this again, because we put the 'ItemName' back to it (letters after '|' character).
									tmp2 = paths[i - 1] + buffer;
								}

								wcsncpy_s(buffer, size, tmp2.c_str(), size - 1);
								buffer[size - 1] = 0;
								//								 //ALLM_IMPORTANT((L"#2 CONSOLIDATED=%s", buffer))
								return;
							}
						}
					}
				}

				wcsncpy_s(buffer, size, target, size - 1);
				buffer[size - 1] = 0;
				//				  ALLM_IMPORTANT((L"#3 CONSOLIDATED=%s", buffer))
			}

			virtual void GetFullName(const wchar_t* name, wchar_t* buffer, size_t size) const
			{
				if (std::wstring(name).find(L"://") != std::wstring::npos)
				{
					wcsncpy_s(buffer, size, name, size - 1);
					buffer[size - 1] = 0;
					return;
				}


				//for (size_t i=0; i<paths.size(); ++i) {
				for (size_t i = paths.size(); i > 0; --i) {
					if (ExistsDispatch(paths[i - 1] + name)) {
						wcsncpy_s(buffer, size, (paths[i - 1] + name).c_str(), size - 1);
						buffer[size - 1] = 0;
						return;
					}
				}

				wcsncpy_s(buffer, size, name, size - 1);
				buffer[size - 1] = 0;
			}

			virtual void GetRelativeName(const wchar_t* name, wchar_t* buffer, size_t size) const
			{
				std::wstring tmp = name;

				for (unsigned int i = 0; i < tmp.size(); ++i) {
					if (tmp[i] == L'\\') tmp[i] = L'/';
				}
				tmp = StringUtil::RemoveDots(tmp);
				StringUtil::ToLowerCase(tmp);

				//	ALLM_IMPORTANT((L"get relative name for %s", tmp.c_str()));
				//we skip the home directory of the game
				for (size_t i = paths.size(); i > 0; --i) {
					//for (size_t i=1; i<paths.size(); ++i) {
					std::wstring pp = paths[i - 1];
					StringUtil::ToLowerCase(pp);
					//ALLM_IMPORTANT((L"checking %s", pp.c_str()));
					if (tmp.find(pp) == 0) {
						//we have a potentical candidate and actually this shouldn't fail or we don't check it for failure at least ;)
						std::wstring relativeName(tmp.begin() + paths[i - 1].length(), tmp.end());
						//ALLM_INFO((L"relative name for %s is %s", name, relativeName.c_str()));
						wcsncpy_s(buffer, size, relativeName.c_str(), size - 1);
						buffer[size - 1] = 0;
						return;
					}
				}

				wcsncpy_s(buffer, size, name, size - 1);
				buffer[size - 1] = 0;

			}

			virtual void GetPrimaryDataPath(wchar_t* buffer, size_t size) const
			{
				if (paths.empty()) {
					throw Exception(L"the resource system doesn't have any paths assigned");
				}

				if (paths.size() > 1) {
					//primary data path
					wcsncpy_s(buffer, size, paths[1].c_str(), size - 1);
				}
				else {
					//start directory of the application
					wcsncpy_s(buffer, size, paths[0].c_str(), size - 1);
				}
				buffer[size - 1] = 0;
			}

			virtual StreamWriter                              CreateStreamWriter(const wchar_t* target)
			{
				return CreateStreamWriterDispatch(target);
			}

			virtual StreamReader                               OpenStream(const wchar_t* name)
			{
				return CreateStreamReader(name);
			}

			com_ptr<Serializer::IRefCountedStreamWriter> CreateSerializerXmlStreamWriter(const wchar_t* target)
			{
				return Serializer::CreateXmlStreamWriter(CreateStreamWriter(target));
			}

			com_ptr<Serializer::IRefCountedStreamReader> CreateSerializerXmlStreamReader(const wchar_t* target)
			{
				return Serializer::CreateXmlStreamReader(CreateStreamReader(target));
			}

			com_ptr<Serializer::IRefCountedStreamWriter> CreateSerializerBinaryStreamWriter(const wchar_t* target)
			{
				return Serializer::CreateBinaryStreamWriter(CreateStreamWriter(target));
			}

			com_ptr<Serializer::IRefCountedStreamReader> CreateSerializerBinaryStreamReader(const wchar_t* target)
			{
				return Serializer::CreateBinaryStreamReader(CreateStreamReader(target));
			}

			com_ptr<Serializer::IRefCountedStreamWriter> CreateSerializerStructuredBinaryStreamWriter(const wchar_t* target)
			{
				return Serializer::CreateStructuredBinaryStreamWriter(CreateStreamWriter(target));
			}

			com_ptr<Serializer::IRefCountedStreamReader> CreateSerializerStructuredBinaryStreamReader(const wchar_t* target)
			{
				return Serializer::CreateStructuredBinaryStreamReader(CreateStreamReader(target));
			}

			com_ptr<Serializer::IRefCountedStreamWriter> CreateDefaultSerializer(const wchar_t* target)
			{
				return Serializer::CreateStructuredBinaryStreamWriter(CreateStreamWriter(target));
			}

			com_ptr<Serializer::IRefCountedStreamReader> CreateDefaultDeserializer_CBF(const wchar_t* target) {
				throw ResourceNotFoundException(L"file not found {0}", IResource::GetRelativeName(target).c_str());
			}

			com_ptr<Serializer::IRefCountedStreamReader> CreateDefaultDeserializer(const wchar_t* target)
			{
				//we should change this other way around.. first trying to load binary and then xml..
				//fixed this 2006.8.1 - juhnu
				StreamReader reader;

				/*  std::wstring binaryTarget=std::wstring(Locator(target).GetPathNoExtension().c_str())+binaryExtension;

				  if (!Exists(binaryTarget.c_str())) {
					  reader=CreateStreamReader(target);
				  } else {
					  reader=CreateStreamReader(binaryTarget.c_str());
				  }*/
				reader = CreateStreamReader(IResource::GetConsolidatedName(target).c_str());

				Serializer::StructuredBinaryStreamHeader header;
				reader->Read(reinterpret_cast<unsigned char*>(&header), sizeof(header));
				reader->SetReadCursor(0, IStream::CursorPosition::Begin);
				if (header.IsValid())
					return Serializer::CreateStructuredBinaryStreamReader(reader);

				// need to chek the stream is xml or not
				unsigned short checksum;
				reader->Read(reinterpret_cast<unsigned char*>(&checksum), sizeof(checksum));
				reader->SetReadCursor(0, IStream::CursorPosition::Begin);
				if (checksum == 0xfeff) // check unicode text
					return Serializer::CreateXmlStreamReader(reader); // assume that the unicode text is just xml
				char ch = 0;
				reader->Read((unsigned char*)(&ch), 1);
				while (ch == 0x09/*tab*/ || ch == 0x0d/*CR*/ || ch == 0x0a/*LF*/ || ch == 0x20/*space*/) // skip white space
					reader->Read((unsigned char*)(&ch), 1);
				if (ch == '<')  // assume that the text with first '<' is ASCII xml
				{
					/* now xml needs an alphabet to assume a xml document */
					reader->Read((unsigned char*)(&ch), 1);
					if (isalpha(ch))
					{
						reader->SetReadCursor(0, IStream::CursorPosition::Begin);
						return Serializer::CreateXmlStreamReader(reader);
					}
				}

				reader->SetReadCursor(0, IStream::CursorPosition::Begin);
				return Serializer::CreateBinaryStreamReader(reader); // none of them, assume that the stream is old binary
			}

			virtual void Reset(const wchar_t* path)
			{
				std::wstring p(path);
				if (p.size() == 0)
				{
					//ALLM_WARNING((L"Resource::Reset() - cannot set Primary Data Path as Empty String. trying to use starting path"));
				}
				else
				{
					startupPath = p;
				}

				paths.clear();
				//ALLM_INFO((L"Resource::Reset() - Paths are cleared."));
				AddPath(startupPath.c_str());
			}

			virtual void AddPath(const wchar_t* path) {
				std::wstring p(path);

				if (p.size() == 0) {
					//ALLM_WARNING((L"empty path ignored"));
					return;
				}

				if (!isAbsolute(p)) {
					p = startupPath + p;
				}

				wchar_t buffer[4096];
				ConsolidatePath(p.c_str(), buffer, 4096);
				p = buffer;

				if (p[int(p.size()) - 1] != L'/')
				{
					paths.push_back(p + L"/");
				}
				else
				{
					paths.push_back(p);
				}
				//ALLM_INFO((L"added path:%s", paths.back().c_str()));
			}

			virtual void Load(const wchar_t* name, const wchar_t** elements, int elementCount, ITextLoaderListener* listener) {
				com_ptr<IStream> stream = OpenStream(name);
				TextLoader loader;
				loader.Load(*stream, name, elements, elementCount, listener);
			}

			virtual void Load(const wchar_t* name, unsigned int* chunkIDs, unsigned int chunkIDCount, IChunkLoaderListener* listener) {
				com_ptr<IStream> stream = OpenStream(name);
				ChunkLoader loader;
				loader.Load(*stream, chunkIDs, chunkIDCount, listener);
			}


			/***************************************************************
			Dispatch-methods
			***************************************************************/

			std::wstring GetProtocol(const std::wstring& target) const {
				Locator locator(target);

				static_wstring<Locator::MaxString> tmp = locator.GetProtocol();

				if (tmp.empty()) return L"file";
				return tmp.c_str();
			}

			bool ExistsDispatch(const std::wstring& target) const {
				StreamFactories::const_iterator i = streamFactories.find(GetProtocol(target));
				if (i != streamFactories.end()) {
					return i->second->Exists(target.c_str());
				}
				throw ResourceNotFoundException(L"cannot find stream opener for the %s-protocol prefix", GetProtocol(target).c_str()), IResource::GetRelativeName(target.c_str()).c_str();

			}

			StreamReader CreateStreamReaderDispatch(const std::wstring& target) {
				StreamFactories::iterator i = streamFactories.find(GetProtocol(target));
				if (i != streamFactories.end()) {
					return i->second->CreateStreamReader(target.c_str());
				}
				throw ResourceNotFoundException(L"cannot find stream opener for the %s-protocol prefix", GetProtocol(target).c_str()), IResource::GetRelativeName(target.c_str()).c_str();
			}

			StreamWriter CreateStreamWriterDispatch(const std::wstring& target) {
				StreamFactories::iterator i = streamFactories.find(GetProtocol(target));
				if (i != streamFactories.end()) {
					return i->second->CreateStreamWriter(target.c_str());
				}
				throw ResourceNotFoundException(L"cannot find stream opener for the %s-protocol prefix", GetProtocol(target).c_str()), IResource::GetRelativeName(target.c_str()).c_str();
			}

			virtual bool Exists(const wchar_t* target) const {
				wchar_t consolidated[4096];

				if (ConsolidatePath(target, consolidated, 4096))
				{
					//consolidated name quaranteed
					return ExistsDispatch(consolidated);	//we have absolute path
				}
				else
				{
					std::wstring tmp = consolidated;
					for (size_t i = paths.size(); i > 0; --i) { //paths are quaranteed to be consolidated.
						//consolidated name quaranteed
						if (ExistsDispatch(paths[i - 1] + tmp)) {
							return true;
						}
					}
					return false;
				}
			}

			virtual StreamReader CreateStreamReader(const wchar_t* name)
			{
				//			  ALLM_IMPORTANT((L"0=%s", name));
				wchar_t consolidated[4096];

				if (ConsolidatePath(name, consolidated, 4096))
				{
					//				  ALLM_IMPORTANT((L"1=%s", consolidated));
					if (ExistsDispatch(consolidated))
					{
						//ALLM_IMPORTANT((L"Opening resource: %s", consolidated));
						return CreateStreamReaderDispatch(consolidated);
					}
				}
				else
				{
					//ALLM_IMPORTANT((L"2=%s", consolidated));
					std::wstring tmp = consolidated;
					for (size_t i = paths.size(); i > 0; --i) { //paths are quaranteed to be consolidated.
						std::wstring tmp2 = paths[i - 1] + tmp;
						if (ExistsDispatch(tmp2)) {
							//ALLM_IMPORTANT((L"Opening resource: %s", tmp2.c_str()));
							//consolidated name quaranteed
							return CreateStreamReaderDispatch(tmp2);
						}
					}
					//ALLM_IMPORTANT((L"3=%s not found", consolidated));
				}
				throw ResourceNotFoundException(L"$Exception.ResourceNotFound$: {0}", IResource::GetRelativeName(name).c_str());
			}

			const wchar_t* GetSerializerBinaryExtension() const
			{
				return binaryExtension.c_str();
			}

			virtual void Update(float dt)
			{
				cacheController.Update(dt);
			}
			virtual ICacheController& GetCacheController()
			{
				return cacheController;
			}

		};
		IResource& ResourceSystemInstance() { return Resource::GetInstance(); }
	}
}