#pragma once
#include <Core/Utils/StringUtil/Compare.h>
#include <Core/FileIO/FileStream.h>
#include "Resource.h"

namespace Lunia {
	namespace Resource {

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
		};
	}
}